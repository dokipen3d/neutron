#version 330 

out vec4 color; 

uniform sampler2DRect texFront;
uniform sampler2DRect texBack;
uniform sampler3D volumeTexture;



void main() {

    //max distance a cube can ever be in unit space.
    const float maxDist = sqrt(3.0);
    //how many samples we want to take (measured along diagonal)
    const int maxNumSamples = 64;
    //calc step inc
    const float stepSize = maxDist/float(maxNumSamples);

    vec4 front = texture(texFront, gl_FragCoord.xy);
    vec4 back = texture(texBack, gl_FragCoord.xy);
    vec3 ray = back.xyz-front.xyz; 
    float rayLength = length(ray);

    //how many samples to take through actual ray.
    float numSamples = rayLength/stepSize;

    //vector we will increment by
    vec3 step = normalize(ray) * stepSize;
    
    //initalize density
    float densitay = 0.0f;
    float T = 1.0f;
    //init start pos
    vec3 pos = front.xyz;
    float col = 0.0f;
    float densityMult = 10;
    //float densityColour = 0.6180;
    float densityColour = 0.32;
    
    //raymarch
    for (int i = 0; i < numSamples; i++, pos += step){
        density = texture(volumeTexture, pos).r * densityMult;
        T *= exp(-density*stepSize);
        col += T*density*stepSize*densityColour;
    }
    
    //normalize by how many samples was possible.
    density /= maxNumSamples;
    density *= 10;
    if(T > 0.95){
        discard;
    }
    //color = vec4(vec3(front.xyz-back.xyz), 1.0); 
    color = vec4(vec3(col), 1.0-T); 
    //color = vec4( vec3(texture(volumeTexture, pos.xyz+vec3(0.5)).xyz), 1.0);
    //color = texture(volumeTexture, front.xyz);
} 

