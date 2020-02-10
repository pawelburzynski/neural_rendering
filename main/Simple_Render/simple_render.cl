const sampler_t samp = CLK_ADDRESS_CLAMP | CLK_NORMALIZED_COORDS_FALSE |
                       CLK_FILTER_LINEAR;

// See the documentations of these functions below

float dist(float4 p1, float4 p2);

__kernel void simple_render(__read_only image3d_t trainingCamImages, // 3D Image with camera array images 
                        __write_only image2d_t dstImage,             // Store the result in this image
                        __constant float *CiPos,                     // (cols x rows) 3x1 vectors with camera positions
                        __constant int *closestCam,                  // precomputed list of closest camera positions
                       __constant float *CurPos,                     // curent camera position
                       int num_datapoints)                           // number of data points

{
    float width = (float)(get_global_id(0));
    float height = (float)(get_global_id(1));
    float4 color = (float4)(0,0,0,0);
    float weight = 0;
    float4 p1 = (float4)(CurPos[0], CurPos[1], CurPos[2], 1);
    for (int i=0; i<num_datapoints; i++) {
        float4 p2 = (float4)(CiPos[closestCam[i]*3+0], CiPos[closestCam[i]*3+1], CiPos[closestCam[i]*3+2], 1);
        float d =dist(p1, p2);
        weight += d;
        color +=  read_imagef(trainingCamImages, samp, (float4)(width+0.5, height+0.5, closestCam[i]+0.5, 0));
    }
    color /= weight;
    int2 dstPos = (int2)(get_global_id(0), get_global_id(1)); // Write pixels at this coordinates
    write_imagef(dstImage, dstPos, color);
}

float dist(float4 p1, float4 p2) {
   return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
}

float gaus(float x) {
    return exp(-x*x*2.0);
}