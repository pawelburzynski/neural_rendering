const sampler_t samp = CLK_ADDRESS_CLAMP | CLK_NORMALIZED_COORDS_FALSE |
                       CLK_FILTER_LINEAR;

// See the documentations of these functions below
float4 mat_mul(__constant float4* A, float4 B);
float aperture_func(float4 wa, float4 camPos, float apertureSize);
float3 display_decode( float3 color_gamma );
float3 display_encode( float3 color_linear );

__kernel void lfrender(__read_only image3d_t lfCamImages, // 3D Image with camera array images 
                        __write_only image2d_t dstImage, // Store the result in this image
                        __constant float *transKCi,    // (cols x rows) 4x4 transformatiom matrices from camera K to array cameras Ci
                        __constant float *transKA,     // 4x4 transformation matrix from from camera K to world coordinates on the aperture plane
                        __constant float *CiPos,       // (cols x rows) 3x1 vectors with camera positions
                        float apertureSize,            // The size of the aperture in the world coordinate space
                        int nrows,                     // Number of rows of cameras in the array
                        int ncols)                     // Number of columns of cameras in the array
{
    float width = (float)(get_global_size(0));
    float height = (float)(get_global_size(1));

    // Coords of the pixel in the rendered virtual camera image
    float4 pk = (float4)((float)get_global_id(0), (float)get_global_id(1), 0.0f, 1.0f);
    
    float4 wa; // Current pixel pk projectoed on the aperture plane

    // Project the pixel coords Pk onto the apperture plane
    wa = mat_mul((__constant float4*)transKA,pk);      
    wa /= wa.w;  // From homogenous to cartesian coordinates

    float3 color = (float3)0.f;
    float weight = 0;
    // For each camera in the array
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) { 
            int camera_index = i*ncols+j;
            // TODO: Transform virtual camera pixel coords pk into array camera coords pi
    
            float4 pi = mat_mul((__constant float4*)(transKCi+camera_index*16),pk);
            pi /= pi.w;
            // TODO: Find the weight of the sample by calling aperture_func
            float4 camPos = (float4)(CiPos[3*camera_index],CiPos[3*camera_index+1],CiPos[3*camera_index+2],0.0);
            float weighti = aperture_func(wa,camPos,apertureSize);
            // TODO: Read a sample from the array camera images lfCamImages. Note that the sample must be "display_decode"d.
            float4 color4 = read_imagef(lfCamImages, samp, (float4)(pi.x+0.5, pi.y+0.5, camera_index+0.5, 0));
            float3 colori = display_decode((float3)(color4.x, color4.y, color4.z));
            // TODO: Add the weighted sample to "color" and update "weight"
            color += colori*weighti;
            weight += weighti;
        }
    }
    color /= weight; // To compute weighted average
    int2 dstPos = (int2)(get_global_id(0), get_global_id(1)); // Write pixels at this coordinates
    float3 colorGamma = display_encode( color ); 
    write_imagef(dstImage, dstPos, (float4)(colorGamma.x, colorGamma.y, colorGamma.z, 1.f));
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

/*
    Function: Matrix4x4 by Vector4 multiplication: A*b

    Matrix A must be in the row-major order.

    Input: C[4] = A[4][4] * B[4] (1x4)    
*/
float4 mat_mul(__constant float4* A, float4 b) {
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