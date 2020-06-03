# Yocto AI Denoising
The main purpose of this project is to integrate and test the [Intel Open Image Denoiser](https://openimagedenoise.github.io) in Yocto. The `yimgdenoise.cpp` app uses the denoiser API to filter noisy images obtained through MC rendering. The parameters that can be passed from the command line to this app are:

- **--input-image**   input noisy image filename
- **--output-image**  output image filename
- **--normal**  normal image filename
- **--albedo**  albedo image filename 
- **--maxMemoryMB** approximate maximum amount of scratch memory to use in megabytes
- **--numThreads** maximum number of threads that can be used by the device
- **--verbose** device verbosity level
- **--setAffinity** bind software threads to hardware threads if set to true (improves performance); false disables binding

The results obtained with the intel denoiser are very convincing and show that this denoiser works well even with images generated using few ssp. The following images are some examples

![image1](./images/classroom.png)

![image2](./images/bistroexterior.png)


## Feature images

### Normal
### Albedo


## Non Local Means


## Test






