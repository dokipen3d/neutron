/*
 * Copyright (c) 2017
 *	Daniel Elliott  All rights reserved.
 *
 * 
 *----------------------------------------------------------------------------
 * Netron SOP
 */

#include "SOP_Neutron.h"

#include <GA/GA_Iterator.h>
#include <GU/GU_Detail.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <SOP/SOP_Guide.h>
#include <SYS/SYS_Math.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <UT/UT_Vector3.h>
#include <iostream>
#include <stddef.h>
#include <PRM/PRM_SpareData.h>
#include "utility_Neutron.h"


using namespace Neutron;

const char* fragShMainSOP = R"foo(
#version 330 
out vec4 color; 

uniform sampler2DRect texFront;
uniform sampler2DRect texBack;
uniform sampler3D volumeTexture;

void main() {
    color = vec4(vec3(1), 1.0); 
}
)foo";

/*
const char* fragShMainSOP2 = R"(
#version 330 
out vec4 color; 
uniform sampler2DRect texFront;
uniform sampler2DRect texBack;
uniform sampler3D volumeTexture;



void main() 
{ 
    vec4 front = texture(texFront, gl_FragCoord.xy);
    vec4 back = texture(texBack, gl_FragCoord.xy);
    vec3 ray = back.xyz - front.xyz;
    
    float stepSize = 0.05;
    float numSteps = length(ray)/stepSize;
    ray = normalize(ray);

    vec4 density = vec4(0.0);
    vec3 pos = front.xyz;

    for (int i = 0; i < 32; i++){
       density = texture(volumeTexture, pos);
       pos += ray*(1.73/32);
    }

    //color = vec4(vec3(density), 1.0);
    color = vec4(vec3(front.xyz), 1.0);
    //color = texture(volumeTexture, pos);

}
)";
*/
 


void newSopOperator(OP_OperatorTable* table)
{
    table->addOperator(new OP_Operator(
        "dokipens_neutron", //internal name
        "Neutron",
        SOP_Neutron::myConstructor,
        SOP_Neutron::myTemplateList,
        0,
        1,
        0,
        OP_FLAG_GENERATOR));
}

static PRM_Name names[] = {
    PRM_Name("enabled", "Turn on and off simulation"),
    PRM_Name("startframe", "Start frame of simulation"),
    PRM_Name("tilesize", "Size of tiles"),
    PRM_Name("shader", "GLSL Shader"),
    PRM_Name("recompile_shader", "Recompile GLSL Shader"),
};

static PRM_Default defaultTileSize(1.0);
static PRM_Default defaultStartFrame(1);
static PRM_Default defaultEnabled(true);
static PRM_Default defaultGLSL(0, fragShMainSOP);


PRM_Template
    SOP_Neutron::myTemplateList[]
    = {

        PRM_Template(PRM_TOGGLE, 1, &names[0], &defaultEnabled),
        PRM_Template(PRM_INT, 1, &names[1], &defaultStartFrame),
        PRM_Template(PRM_FLT, 1, &names[2], &defaultTileSize),
        PRM_Template(PRM_STRING, 1, &names[3], &defaultGLSL, 0, 0, 0, &PRM_SpareData::stringEditor),
        PRM_Template(PRM_CALLBACK,  1, &names[4], 0, 0, 0, SOP_Neutron::recompileShader),
        PRM_Template(),
      };



OP_Node*
SOP_Neutron::myConstructor(OP_Network* net, const char* name, OP_Operator* op)
{
    return new SOP_Neutron(net, name, op);
}

SOP_Neutron::SOP_Neutron(OP_Network* net, const char* name, OP_Operator* op)
    : SOP_Node(net, name, op)
{
    // This indicates that this SOP manually manages its data IDs,
    // so that Houdini can identify what attributes may have changed,
    // e.g. to reduce work for the viewport, or other SOPs that
    // check whether data IDs have changed.
    // By default, (i.e. if this line weren't here), all data IDs
    // would be bumped after the SOP cook, to indicate that
    // everything might have changed.
    // If some data IDs don't get bumped properly, the viewport
    // may not update, or SOPs that check data IDs
    // may not cook correctly, so be *very* careful!
    mySopFlags.setManagesDataIDs(false);

    // Make sure to flag that we can supply a guide geometry
    mySopFlags.setNeedGuide1(true);
    std::cout << "con!\n";
    cachedGDP = new GU_Detail();
    // Try with a float

    myFluid::simvec.push_back(mySim{});
    

    
    

}


int SOP_Neutron::recompileShader(void *data, int index,  
    float time, const PRM_Template *tplate ){

        SOP_Neutron *me = static_cast<SOP_Neutron*>(data);
        
        //me->calledFromCallback = true;
        //me->myCallBackFlags = 0;
        
        OP_Context myContext(time);
        myContext.setData(static_cast<OP_ContextData*>(data));

        //eval the string parm from the node
        UT_String shader;
        me->evalString(shader, "shader", 0, time); 
            
        GA_RWHandleI uniqueHandleID(me->cachedGDP, GA_ATTRIB_DETAIL, "uniqueHandleID");
        //create a reference to the sim object associated with this SOP
        mySim& simObject = myFluid::simvec[uniqueHandleID.get(0)];
        simObject.fragmentShader = std::string(shader.steal());
        //std::cout << simObject.fragmentShader << "\n";
        simObject.shaderNeedsRecompile = true;
        
        me->cookMe(myContext);
        std::cout << "callback\n";
        return 1;
        

}

SOP_Neutron::~SOP_Neutron() {}

OP_ERROR
SOP_Neutron::cookMySop(OP_Context& context)

{
    
    gdp->clearAndDestroy();
    OP_Node::flags().timeDep = 1;
    SOP_Node::flags().forceCook = 1;

    fpreal now = context.getTime();
    std::cout << "time is " << now << "\n";
    std::cout << "time elapsed is " << now - last << "\n";
    std::cout << std::endl;
    last = now;
    //sOP_Node::flags().alwaysCook = 1;
    
        
    
    if(!initialized){
            std::cout << "not init\n";
            cachedGDP->appendPrimitive(GA_PRIMPOLYSOUP);
            attrib = cachedGDP->findFloatTuple(GA_ATTRIB_DETAIL, "__fluid__");
            // Not present, so create the detail attribute:
            if (!attrib.isValid()) {
                attrib = GA_RWHandleS(cachedGDP->addStringTuple(GA_ATTRIB_DETAIL, "__fluid__", 1));
            }
        //if not set then when checking in the GUI renderhook then it wont be valid
            attrib.set(0,"ON");

            initialized = true;
            uniqueIndex = GA_RWHandleI(cachedGDP->addIntTuple(GA_ATTRIB_DETAIL,"uniqueHandleID",1 ));
            uniqueIndex.set(0,myFluid::simvec.size()-1);
    }

    gdp->copy(*cachedGDP, GEO_COPY_ONCE, true, false, GA_DATA_ID_CLONE);
    //gdp = cachedGDP;
    //GA_DATA_ID_BUMP
    //GA_DATA_ID_CLONE
    

    return error();
}
OP_ERROR
SOP_Neutron::cookMyGuide1(OP_Context& context)
{
    return error();
}


// const char *
// SOP_Neutron::inputLabel(unsigned inum) const
// {
//
//     return "input solver nodes";
// }
