const sampler_t samp = CLK_ADDRESS_CLAMP | CLK_NORMALIZED_COORDS_FALSE |
                       CLK_FILTER_LINEAR;

// See the documentations of these functions below
float4 mat_mul_g(__global float4* A, float4 B);
float4 mat_mul_c(__constant float4* A, float4 B);
float aperture_func(float4 wa, float4 camPos, float apertureSize);
float dist(float4 p1, float4 p2);
float gaus(float x);
float3 display_decode( float3 color_gamma );
float3 display_encode( float3 color_linear );

__kernel void projection_renderer(__read_only image3d_t trainingCamImages,  // 3D Image with camera array images 
                        __write_only image2d_t dstImage,                // Store the result in this image
                        __constant float *CiPos,                        // (cols x rows) 3x1 vectors with camera positions
                        __constant float *CurPos,                       // curent camera position
                        __constant int *closestCam,                     // precomputed list of closest camera positions
                        int num_datapoints,                             // number of data points
                        __global const float *proMat,                   // 4x4 projection matrices for all training camera inputs
                        __constant float *invProMatCam)                 // inverse projection camera matrix of current camera  
{
    float width = (float)(get_global_id(0));
    float height = (float)(get_global_id(1));
    float3 color = (float3)0.f;
    float weight = 0;
    float4 cam_pos = (float4)(CurPos[0], CurPos[1], CurPos[2], 1);
    //projection
    float4 pix_pos = mat_mul_c((__constant float4*)invProMatCam,(float4)(width,height,1.0,0.0));
    pix_pos/=pix_pos.w;
    float3 plane_normal = (float3)(0.0,0.0,1.0);
    float4 ray_dir_4 = pix_pos-cam_pos;
    float3 cam_pos_3 = (float3)(cam_pos.x, cam_pos.y,cam_pos.z);
    float3 ray_dir = (float3)(ray_dir_4.x, ray_dir_4.y, ray_dir_4.z);
    float d = 0;
    float s = - (d + dot(plane_normal,cam_pos_3))/(dot(plane_normal,ray_dir));
    float4 projection = cam_pos+s*ray_dir_4;


    for (int i=0; i<num_datapoints; i++) {
        float4 p2 = (float4)(CiPos[closestCam[i]*3+0], CiPos[closestCam[i]*3+1], CiPos[closestCam[i]*3+2], 1);
        float d = dist(cam_pos, p2);
        float weighti =  gaus(d);
        //float4 pix_pos_i = mat_mul_g((__global float4*)(proMat+16*closestCam[i]),projection);
        float4 const pix_pos_i = proMat[closestCam[i]];
        //pix_pos_i/=pix_pos_i.z;
        float4 color4  =  read_imagef(trainingCamImages, samp, (float4)(width+0.5, height+0.5, closestCam[i]+0.5, 0));
        float3 colori = display_decode((float3)(color4.x, color4.y, color4.z));
        if (get_global_id(0) == 300 && get_global_id(1) == 600 && i==0) {
            printf("Closest camera: %d dist: %lf\n", closestCam[i], dist(cam_pos,p2));
            printf("cam_pos %lf %lf %lf\n", cam_pos.x, cam_pos.y, cam_pos.z);    
            printf("pix_pos_i %lf %lf %lf %lf\n", pix_pos_i.x, pix_pos_i.y, pix_pos_i.z, pix_pos_i.w);
        }
        color += colori*weighti;
        weight += weighti;
    }
    color /= weight;
    int2 dstPos = (int2)(get_global_id(0), get_global_id(1)); // Write pixels at this coordinates
    float3 colorGamma = display_encode( color ); 
    write_imagef(dstImage, dstPos, (float4)(colorGamma.x, colorGamma.y, colorGamma.z, 1.f));
}

float dist(float4 p1, float4 p2) {
   return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
}

float gaus(float x) {
    return exp(-x*x*2.0);
}

/*
    Function: Calculate sample weight as the function of distance from the camera centre

    Input:
    camPos - position of the camera in the array in the world coordinates
    wa - point on aperture plane
    apertureSize - size of the aperture

    Return: sample weight
*/
float aperture_func(float4 wa, float4 camPos, float apertureSize)
{
    // The function should have a cone shape with the tip at 1 at camPos and 
    // a circular base at 0 of the diameter "apertureSize"
    float d = sqrt((camPos.x-wa.x)*(camPos.x-wa.x)+(camPos.y-wa.y)*(camPos.y-wa.y))/apertureSize;
    if (d>=1) {
        return 0.0f;
    } else {
        return 1.0-d;
    }
}

float4 mat_mul_g(__global float4* A, float4 b) {
    float4 C;
    float *Cf = (float*)&C;
    for (int n = 0; n < 4; n++) {
        Cf[n] = dot(A[n],b);
    }
    return C;
}

float4 mat_mul_c(__constant float4* A, float4 b) {
    float4 C;
    float *Cf = (float*)&C;
    for (int n = 0; n < 4; n++) {
        Cf[n] = dot(A[n],b);
    }
    return C;
}

/*
  Convert from gamma-corrected (display encoded) to linear colour.
 */
float3 display_decode( float3 color_gamma )
{
    const float gamma = 2.2f;
    return native_powr( color_gamma, (float3)gamma );
}

/*
  Convert from linear to gamma-corrected (display emcoded) colour.
 */
float3 display_encode( float3 color_linear )
{
    const float gamma = 2.2f;
    return native_powr( color_linear, (float3)(1.f/gamma) );
}