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

  int Ds = 3;
  int ds = 3;
  float sigma_s = 0.0f;
  float sigma_r = 0.0f;
  float k = 0.0f;


  auto cli = cli::make_cli("ynlmdenoise", "Non Local Means Denoiser app");
  
  cli::add_option(cli, "--input-image,-i", input_filename, "Input image filename", true);
  cli::add_option(cli, "--output-image,-o", output_filename, "Output image filename", true);
  cli::add_option(cli, "--albedo,-a", albedo_filename, "Albedo image filename", true); 
  cli::add_option(cli, "--normal,-n", normal_filename, "Normal image filename", true); 
  cli::add_option(cli, "--Ds", Ds, "Search window side half-length");
  cli::add_option(cli, "--ds", ds, "Patch side half-length");
  cli::add_option(cli, "--sigma-s", sigma_s, "Adjust distance across image", true);
  cli::add_option(cli, "--sigma-r", sigma_r, "Adjust distance across color space", true);
  cli::add_option(cli, "--strength,-k", k, "Strength of the filter", true);
  cli::parse_cli(cli, argc, argv);


  
  // input image
  img::image<vec3f> img = {};
  img::image<vec3f> albedo_image = {};
  img::image<vec3f> normal_image = {};

  auto ioerror = ""s;
  
  img::load_image(input_filename, img, ioerror);
  auto size = img.size();

  if (! albedo_filename.empty()) {
    img::load_image(albedo_filename, albedo_image, ioerror);
  }

  if (! normal_filename.empty()) {
    img::load_image(normal_filename, normal_image, ioerror);
  }
  
  auto res = ext::nlm_denoise(img , albedo_image, normal_image, Ds, ds, sigma_s, sigma_r, k);
  img::save_image(output_filename, res, ioerror);

  return 0;

}