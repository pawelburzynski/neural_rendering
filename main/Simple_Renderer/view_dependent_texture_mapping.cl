const sampler_t samp = CLK_ADDRESS_CLAMP | CLK_NORMALIZED_COORDS_FALSE |
                       CLK_FILTER_LINEAR;

float4 mat_mul(__constant float4* A, float4 B);
float dist(float4 p1, float4 p2);
float gaus(float x);
float3 display_decode( float3 color_gamma );
float3 display_encode( float3 color_linear );
float plane_projection( float3 plane_normal, float d, float4 p1, float4 p2);
float sphere_projection( float3 sphere_center, float r, float4 p1, float4 p2);
bool inside_view(float x, float y, float width, float height);

__kernel void view_dependent_texture_mapping(__read_only image3d_t trainingCamImages,  // 3D Image with camera array images 
                        __write_only image2d_t dstImage,                // Store the result in this image
                        __constant float *CiPos,                        // (cols x rows) 3x1 vectors with camera positions
                        __constant float *CurPos,                       // curent camera position
                        __constant const float *proMat,                   // 4x4 projection matrices for all training camera inputs
                        __constant float *invProMatCam,                 // inverse projection camera matrix of current camera  
                        __constant int *closestCam,                     // precomputed list of closest camera positions
                        __private int num_datapoints)                             // number of data points
{
    float width = (float)(get_global_id(0));
    float height = (float)(get_global_id(1));
    float3 color = (float3)0.f;
    float weight = 0;
    float4 cam_pos = (float4)(CurPos[0], CurPos[1], CurPos[2], 1);
    //projection
    float4 pix_pos = mat_mul((__constant float4*)invProMatCam,(float4)(width,height,1.0,0.0));
    pix_pos/=pix_pos.w;
    float3 plane_normal = (float3)(0.0,0.0,1.0);
    float d = 0;
    float s1 = plane_projection(plane_normal,d,cam_pos,pix_pos);
    float3 sphere_center = (float3)(0.0,0.0,2.0);
    float r = 2.0;
    float s2 = sphere_projection(sphere_center, r, cam_pos, pix_pos);
    float s = s1;
    if( s2 >= 0 && s2 < s1 ) 
    {
        s = s2;
    }
    float4 projection = cam_pos+s*(pix_pos-cam_pos);

    for (int i=0; i<num_datapoints; i++) 
    {
        float4 p2 = (float4)(CiPos[closestCam[i]*3+0], CiPos[closestCam[i]*3+1], CiPos[closestCam[i]*3+2], 1);
        float d = dist(cam_pos, p2);
        float weighti =  gaus(d);
        float4 pix_pos_i = mat_mul((__constant float4*)(proMat+16*closestCam[i]),projection);
        pix_pos_i/=pix_pos_i.z;
        float4 color4  =  read_imagef(trainingCamImages, samp, (float4)(pix_pos_i.x+0.5, pix_pos_i.y+0.5, closestCam[i]+0.5, 0));
        float3 colori = display_decode((float3)(color4.x, color4.y, color4.z));
        if (get_global_id(0) == 300 && get_global_id(1) == 100 && i==0) 
        {
            printf("cam_pos %lf %lf %lf\n", cam_pos.x, cam_pos.y, cam_pos.z);    
            printf("pix_pos_i %lf %lf %lf %lf\n", pix_pos.x, pix_pos.y, pix_pos.z, pix_pos.w);
        }
        if(inside_view(pix_pos_i.x, pix_pos_i.y, get_image_width(trainingCamImages), get_image_height(trainingCamImages))) 
        {
            color += colori*weighti;
            weight += weighti;
        }
    }
    color /= weight;
    int2 dstPos = (int2)(get_global_id(0), get_global_id(1)); // Write pixels at this coordinates
    float3 colorGamma = display_encode( color ); 
    write_imagef(dstImage, dstPos, (float4)(colorGamma.x, colorGamma.y, colorGamma.z, 1.f));
}

float4 mat_mul(__constant float4* A, float4 b) 
{
    float4 C;
    float *Cf = (float*)&C;
    for (int n = 0; n < 4; n++) 
    {
        Cf[n] = dot(A[n],b);
    }
    return C;
}

float dist(float4 p1, float4 p2) 
{
   return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
}

float gaus(float x) 
{
    return exp(-x*x*2.0);
}

float plane_projection( float3 plane_normal, float d, float4 p1, float4 p2) 
{
    float4 ray_dir_4 = p2-p1;
    float3 cam_pos_3 = (float3)(p1.x, p1.y,p1.z);
    float3 ray_dir = (float3)(ray_dir_4.x, ray_dir_4.y, ray_dir_4.z);
    float s = - (d + dot(plane_normal,cam_pos_3))/(dot(plane_normal,ray_dir));
    return s;
}

float sphere_projection( float3 sphere_center, float r, float4 p1, float4 p2) 
{
    float4 ray_dir_4 = p2-p1;
    float3 cam_pos_3 = (float3)(p1.x, p1.y,p1.z);
    float3 ray_dir = (float3)(ray_dir_4.x, ray_dir_4.y, ray_dir_4.z);
    float a = dot(ray_dir,ray_dir);
    float b = dot(2*ray_dir,(cam_pos_3-sphere_center));
    float c = dot(cam_pos_3-sphere_center,cam_pos_3-sphere_center)-r*r;
    float d = b*b-4*a*c;
    if( d < 0 ) 
    {
        return -1;
    }
    float s1 = (-b-sqrt(d))/(2*a);
    float s2 = (-b+sqrt(d))/(2*a);
    if(s1>s2) 
    {
        float s3 = s1;
        s1 = s2;
        s2 = s3;
    }
    if( s1 < 0 ) 
    {
        return s2;
    }
    return s1;
}

float3 display_decode( float3 color_gamma )
{
    const float gamma = 2.2f;
    return native_powr( color_gamma, (float3)gamma );
}

float3 display_encode( float3 color_linear )
{
    const float gamma = 2.2f;
    return native_powr( color_linear, (float3)(1.f/gamma) );
}

bool inside_view(float x, float y, float width, float height)
{
    if ( x < -10 || y < -10 ) {
        return false;
    }

    if ( x > width+10 || y > height+10 ) {
        return false;
    }
    return true;
}