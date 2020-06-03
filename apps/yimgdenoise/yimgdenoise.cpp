#include<stdio.h>
#include<yocto/yocto_image.h>
#include<yocto/yocto_commonio.h>

#include "ext/oidn/include/OpenImageDenoise/oidn.hpp"

using namespace std;
using namespace yocto::math;
namespace img = yocto::image;
namespace cli = yocto::commonio;



void device_error_callback(void* userPtr, oidn::Error error, const char* message)
{
  throw std::runtime_error(message);
}


int main(int argc, const char* argv[]) {

  // command line options  
  string color_filename;
  string albedo_filename;
  string normal_filename;
  string output_filename;

  int numThreads = -1;
  int verbose = -1;
  int maxMemoryMB = -1;

  // bind software threads to hardware threads 
  // if set to true (improves performance); false disables binding
  int affinity = 1;

  auto cli = cli::make_cli("yimgdenoise", "Intel Open Image Denoiser app");
  
  // mandatory
  cli::add_option(cli, "--input-image,-i", color_filename, "Input image filename", true);
  cli::add_option(cli, "--output-image,-o", output_filename, "Output image filename", true);

  // optional
  cli::add_option(cli, "--albedo,-a", albedo_filename, "Albedo image filename"); 
  cli::add_option(cli, "--normal,-n", normal_filename, "Normal image filename"); 
  cli::add_option(cli, "--maxMemoryMB,-m", maxMemoryMB, "Maximum amount of memory");
  cli::add_option(cli, "--numThreads,-t", numThreads, "Number of threads");
  cli::add_option(cli, "--verbose", verbose, "Verbosity");
  cli::add_option(cli, "--setAffinity", affinity, "Affinity");
  cli::parse_cli(cli, argc, argv);
  

  // create a device and set its parameters
  oidn::DeviceRef device = oidn::newDevice(oidn::DeviceType::CPU);
  if (numThreads >= 0) device.set("numThreads", numThreads);
  if (verbose >= 0) device.set("verbose", verbose);
  
  device.set("setAffinity", (bool)affinity);

  const char* device_error_message;
  if (device.getError(device_error_message) != oidn::Error::None)
    throw std::runtime_error(device_error_message);

  device.setErrorFunction(device_error_callback);
  device.commit();

  printf("Device parameters:\n");
  printf("numThreads  : %d\n", device.get<int>("numThreads"));
  printf("verbose     : %d\n", device.get<int>("verbose"));
  printf("setAffinity : %d\n", device.get<bool>("setAffinity"));
  printf("\n");

  // create a filter and set its parameters
  oidn::FilterRef filter = device.newFilter("RT");

  // input image
  img::image<vec3f> color_image = {};
  // feature images
  img::image<vec3f> normal_image = {};
  img::image<vec3f> albedo_image = {};

  string error = color_filename +  " load error";
  img::load_image(color_filename, color_image, error);
  auto size = color_image.size();

  // prepare output image
  img::image<vec3f> output_image = img::image<vec3f>(size);
  filter.setImage("color", color_image.data(), oidn::Format::Float3, size.x, size.y);
  filter.setImage("output", output_image.data(), oidn::Format::Float3, size.x, size.y);

  if (! albedo_filename.empty()) {
    string albedo_error = albedo_filename +  " load error";
    img::load_image(albedo_filename, albedo_image, albedo_error);
    //assert(size == albedo_image.size());
    printf("setting albedo img feature\n");
    filter.setImage("albedo", albedo_image.data(), oidn::Format::Float3, size.x, size.y); // optional
  }

  if (! normal_filename.empty()) {
    string normal_error = normal_filename +  " load error";
    img::load_image(normal_filename, normal_image, normal_error);
    //assert(size == normal_image.size());
    printf("setting normal img feature\n");
    filter.setImage("normal", normal_image.data(), oidn::Format::Float3, size.x, size.y); // optional
  }
  
  printf("Input image file: %s\n", color_filename.c_str());
  auto is_hdr = img::is_hdr_filename(color_filename);
  printf("image is hdr: %d\n", is_hdr);
  filter.set("hdr", is_hdr);

  if (maxMemoryMB >= 0) {
    filter.set("maxMemoryMB", maxMemoryMB);
  }

  filter.commit();

  // filter and save the image
  filter.execute();
  img::save_image(output_filename, output_image, error);
  
  return 0;


}