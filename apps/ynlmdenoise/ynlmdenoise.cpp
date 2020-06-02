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

  
    // command line options  
  string input_filename;
  string albedo_filename;
  string normal_filename;
  string output_filename;

  int Ds = 0;
  int ds = 0;
  float sigma_s = 0.0f;
  float sigma_r = 0.0f;
  float k = 0.0f;


  auto cli = cli::make_cli("ynlmdenoise", "Non Local Means Denoiser app");
  
  // mandatory
  cli::add_option(cli, "--input-image,-i", input_filename, "Input image filename", true);
  cli::add_option(cli, "--output-image,-o", output_filename, "Output image filename", true);
  cli::add_option(cli, "--Ds", Ds, "Patch side half-length", true);
  cli::add_option(cli, "--ds", ds, "Output image filename", true);
  cli::add_option(cli, "--sigma-s", sigma_s, "Adjust distance across image", true);
  cli::add_option(cli, "--sigma-r", sigma_r, "Adjust distance across color space", true);
  cli::add_option(cli, "--strength,-k", k, "Strength of the filter", true);
  
  // optional
  cli::add_option(cli, "--albedo,-a", albedo_filename, "Albedo image filename"); 
  cli::add_option(cli, "--normal,-n", normal_filename, "Normal image filename"); 
  cli::parse_cli(cli, argc, argv);




  // input image
  img::image<vec3f> img = {};

  string error = "load error";
  img::load_image(input_filename, img, error);
  auto size = img.size();
  
  auto res = ext::nlm_denoise(img , Ds, ds, sigma_s, sigma_r, k);
  img::save_image(output_filename, res, error);

  return 0;

}
