//
// Implementation for Yocto/Extension.
//

//
// LICENSE:
//
// Copyright (c) 2020 -- 2020 Fabio Pellacini
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "yocto_extension.h"

#include <atomic>
#include <deque>
#include <future>
#include <memory>
#include <mutex>

#include <math.h>


using namespace std::string_literals;

// -----------------------------------------------------------------------------
// MATH FUNCTIONS
// -----------------------------------------------------------------------------
namespace yocto::extension {

// import math symbols for use
using math::abs;
using math::acos;
using math::atan2;
using math::clamp;
using math::cos;
using math::exp;
using math::flt_max;
using math::fmod;
using math::fresnel_conductor;
using math::fresnel_dielectric;
using math::identity3x3f;
using math::invalidb3f;
using math::log;
using math::make_rng;
using math::max;
using math::min;
using math::pif;
using math::pow;
using math::sample_discrete_cdf;
using math::sample_discrete_cdf_pdf;
using math::sample_uniform;
using math::sample_uniform_pdf;
using math::sin;
using math::sqrt;
using math::zero2f;
using math::zero2i;
using math::zero3f;
using math::zero3i;
using math::zero4f;
using math::zero4i;

}  // namespace yocto::pathtrace

// -----------------------------------------------------------------------------
// IMPLEMENTATION FOR EXTENSION
// -----------------------------------------------------------------------------
namespace yocto::extension {
    

    img::image<vec3f> reflection_padding(img::image<vec3f> img, int border) {
        auto size = img.size();
        
        auto new_size = zero2i;
        new_size.x = size.x + 2*border;
        new_size.y = size.y + 2*border;
        auto result = img::image<vec3f>(new_size);

        for (auto j = 0; j < size.y; j++) {
            for (auto i = 0; i < size.x; i++) {
                //printf("%f, %f, %f\n", img[{i, j}].x, img[{i, j}].y, img[{i, j}].z );
                result[{i+border, j+border}] = img[{i,j}];
            }
        }

        // left border
        for (auto j=border; j < new_size.y - border; j++) {
            for (auto i=0; i < border; i++) {
                result[{i, j}] = result[{2*border -i, j}];
            }   
        }

        // right border
        for (auto j=border; j < new_size.y - border; j++) {
            for (auto i=new_size.x -border; i < new_size.x; i++) {
                result[{i, j}] = result[{new_size.x-border - (i + border - new_size.x) -1 ,j}];
            }
        }

        // upper border
        for (auto i=border; i < new_size.x - border; i++) {
            for (auto j=0; j < border; j++) {
                result[{i, j}] = result[{i, 2*border -j}];
            }
        }

        // bottom border
        for (auto i=border; i < new_size.x - border; i++) {
            for (auto j=new_size.y -border; j < new_size.y; j++) {
                result[{i, j}] = result[{i , new_size.y - border - (j + border - new_size.y) -1}];
            }
        }


        // corners
        for (auto j=new_size.y -border; j < new_size.y; j++) {
            for (auto i=0; i < border; i++) {
                result[{i, j}] =  result[{2*border -i, j}];
            }
        }

        for (auto j=0; j < border; j++) {
            for (auto i=0; i < border; i++) {
                result[{i, j}] =  result[{2*border -i, j}];
            }
        }

        for (auto j=new_size.y - border; j < new_size.y; j++) {
            for (auto i=new_size.x - border; i < new_size.x; i++) {
                result[{i, j}] = result[{new_size.x-border - (i + border - new_size.x) -1 ,j}];
            }
        }

        for (auto j=0; j < border; j++) {
            for (auto i=new_size.x - border; i < new_size.x; i++) {
                result[{i, j}] = result[{new_size.x-border - (i + border - new_size.x) -1 ,j}];
            }
        }


        return result;
    }



        img::image<vec3f> nlm_denoise(img::image<vec3f> img, int Ds, int ds, int h) { // TODO: check parameter h
        // initialization
        auto size = img.size();
        auto D = 2*Ds +1;
        auto d = 2*ds +1;
        auto nc = 3;

        auto result = img::image<vec3f>(size);
        auto ref_img = reflection_padding(img, Ds + ds);

        printf("size: y=%d, x=%d\n", img.size().y, img.size().x );
        printf("reflected size: y=%d, x=%d\n", ref_img.size().y, ref_img.size().x );

        // K = compute kernel

        auto weights = std::vector<float>();

        for (auto x1 = Ds+ds; x1 < ref_img.size().y - (Ds+ds); x1++) {
            for (auto x2 = Ds+ds; x2 < ref_img.size().x - (Ds+ds); x2++) {
                weights.clear();  
                
                
                for (auto y1 = x1-Ds; y1 < x1+Ds; y1++) {
                    for (auto y2 = x2-Ds; y2 < x2+Ds; y2++) {
                        auto dist2 = 0;

                        for (auto z1 = -ds; z1 < ds; z1++) {
                            for (auto z2 = -ds; z2 < ds; z2++) {

                                printf("x1 + z1 %d\n", x1 + z1);           
                                printf("x2 + z2 %d\n", x2 + z2);   
                                printf("y1 + z1 %d\n", y1 + z1);                           
                                printf("y2 + z2 %d\n", y2 + z2);
                                
                                dist2 += (1/ d*d) * pow(ref_img[{x2 + z2, x1 + z1}].x - ref_img[{y2 + z2, x1 + z1}].x, 2);
                                dist2 += (1/ d*d) * pow(ref_img[{x2 + z2, x1 + z1}].y - ref_img[{y2 + z2, x1 + z1}].y, 2);
                                dist2 += (1/ d*d) * pow(ref_img[{x2 + z2, x1 + z1}].z - ref_img[{y2 + z2, x1 + z1}].z, 2);
                            }
                        }

                        // compute weights
                        weights.push_back(exp(dist2 / (nc * pow(h, 2))));
                    }
                }

                auto r = zero3f;
                auto s = 0.0f;
                auto i=0;
                for (auto y1 = x1-Ds; y1 < x1+Ds; y1++) {
                     for (auto y2 = x2-Ds; y2 < x2+Ds; y2++) {
                         r.x += weights[i] * ref_img[{y2, y1}].x;
                         r.y += weights[i] * ref_img[{y2, y1}].y;
                         r.z += weights[i] * ref_img[{y2, y1}].z;
                         s += weights[i];
                         i += 1;
                     }
                }

                result[{x2 - (Ds + ds), x1 - (Ds + ds)}].x = clamp(r.x / s, 0.0, 1.0);
                result[{x2 - (Ds + ds), x1 - (Ds + ds)}].y = clamp(r.y / s, 0.0, 1.0);
                result[{x2 - (Ds + ds), x1 - (Ds + ds)}].z = clamp(r.z / s, 0.0, 1.0);
            }
        }


        //return ref_img;
        return result;
    }


























}  // namespace yocto::pathtrace
