/*
 * Copyright (c) 2017
 *	Daniel Elliott  All rights reserved.
 *
 * 
 *----------------------------------------------------------------------------
 * Netron SOP
 */

#include "SOP_Neutron.h"

#include <SOP/SOP_Guide.h>
#include <GU/GU_Detail.h>
#include <GA/GA_Iterator.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <UT/UT_Vector3.h>
#include <SYS/SYS_Math.h>
#include <stddef.h>
#include <iostream>

using namespace Neutron;

void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "dokipens_neutron",					//internal name
        "Neutron",
        SOP_Neutron::myConstructor,
        SOP_Neutron::myTemplateList,
       	0,
        1,
		0,
		OP_FLAG_GENERATOR));
}

static PRM_Name names[] = {
    PRM_Name("enabled",	"Turn on and off simulation"),
    PRM_Name("startframe",	"Start frame of simulation"),
    PRM_Name("tilesize",	"Size of tiles"),
};

static PRM_Default  defaultTileSize(1.0);
static PRM_Default  defaultStartFrame(1);
static PRM_Default  defaultEnabled(true);





PRM_Template
SOP_Neutron::myTemplateList[] = {
    
    PRM_Template(PRM_TOGGLE, 1, &names[0], &defaultEnabled),
    PRM_Template(PRM_INT,    1, &names[1], &defaultStartFrame),
    PRM_Template(PRM_FLT,    1, &names[2], &defaultTileSize),
    PRM_Template(),
};


OP_Node *
SOP_Neutron::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Neutron(net, name, op);
	
}

SOP_Neutron::SOP_Neutron(OP_Network *net, const char *name, OP_Operator *op)
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
	
}

SOP_Neutron::~SOP_Neutron() {}





OP_ERROR
SOP_Neutron::cookMySop(OP_Context &context)
	
	
{

	OP_Node::flags().timeDep = 1;
		
	fpreal now = context.getTime();
	std::cout << "time is " << now << "\n";
	std::cout << "time elapsed is " << now - last << "\n";
	std::cout << std::endl;
	last = now;
	OP_Node::flags().forceCook = 1;
	//sOP_Node::flags().alwaysCook = 1;
	
	
	
	
    return error();
}
OP_ERROR
SOP_Neutron::cookMyGuide1(OP_Context &context)
{
    return error();
	
}
// const char *
// SOP_Neutron::inputLabel(unsigned inum) const
// {
//
//     return "input solver nodes";
// }