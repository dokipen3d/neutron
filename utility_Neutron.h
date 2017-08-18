
#ifndef __UTILITY_Neutron_h__
#define __UTILITY_Neutron_h__

#include <vector>
#include <string>

class mySim{
public:
    
    std::string fragmentShader = R"(
        #version 330 
        
        out vec4 color; 
        
        uniform sampler2DRect texFront;
        uniform sampler2DRect texBack;
        uniform sampler3D volumeTexture;
        
        
        
        void main() {
        
            //max distance a cube can ever be in unit space.
            const float maxDist = sqrt(3.0);
            //how many samples we want to take (measured along diagonal)
            const int maxNumSamples = 32;
            //calc step inc
            const float stepSize = maxDist/float(maxNumSamples);
        
            vec4 front = texture(texFront, gl_FragCoord.xy);
            vec4 back = texture(texBack, gl_FragCoord.xy);
            vec3 ray = front.xyz-back.xyz; 
            float rayLength = length(ray);
        
            //how many samples to take through actual ray.
            float numSamples = rayLength/stepSize;
        
            //vector we will increment by
            vec3 step = normalize(ray) * stepSize;
            
            //initalize density
            float density = 0.0f;
            float T = 1.0f;
            //init start pos
            vec3 pos = back.xyz;
            float col = 0.0f;
            float densityMult = 10;
            //float densityColour = 0.6180;
            float densityColour = 0.32;
            float d2 = 0.0f;
            float T2 = 1.0f;
            
            //raymarch
            for (int i = 0; i < numSamples; i++, pos += step){
            
                density = texture(volumeTexture, pos).r * densityMult;
                if(density > 0.01){
                T *= exp(-density*stepSize);
                //d2 += density;
                col += T*density*stepSize*densityColour;
                }
                if(T < 0.01){
                //break;
                }
            }
            
            //normalize by how many samples was possible.
            //density /= maxNumSamples;
            //density *= 10;
            
            //T2 = exp(-d2/maxNumSamples);
            //color = vec4(vec3(front.xyz-back.xyz), 1.0); 
            color = vec4(vec3(col), 1.0-T); 
            //color = vec4(1.0);
            //color = vec4(vec3(1.0),T);
            //color = vec4(vec3(col), 1.0-T2);
            //color = vec4( vec3(texture(volumeTexture, pos.xyz+vec3(0.5)).xyz), 1.0);
            //color = texture(volumeTexture, front.xyz);
        }         
    )";

    bool shaderNeedsRecompile = true;
    void doSomething();
};


class myFluid{

public:

    static std::vector<mySim> simvec;
    

};



#endif