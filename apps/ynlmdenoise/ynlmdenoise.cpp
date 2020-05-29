#include<stdio.h>
#include<yocto/yocto_image.h>
#include<yocto/yocto_commonio.h>
#include<yocto_extension/yocto_extension.h>

using namespace std;
using namespace yocto::math;
namespace img = yocto::image;
namespace cli = yocto::commonio;
namespace ext = yocto::extension;


int main(int argc, const char* argv[]) {

  string image_filename = "out/bathroom/bathroom_1280_2048.jpg";
  


  // input image
  img::image<vec3f> img = {};

  string error = "load error";
  img::load_image(image_filename, img, error);
  auto size = img.size();
  
  auto res = ext::nlm_denoise(img , 10, 2, 1);
  img::save_image("output.jpg", res, error);

  return 0;

}
