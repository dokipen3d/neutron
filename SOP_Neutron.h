/*
 * Copyright (c) 2017
 *	Daniel Elliott  All rights reserved.
 *
 * 
 *----------------------------------------------------------------------------
 * Netron SOP
 */


#ifndef __SOP_Neutron_h__
#define __SOP_Neutron_h__

#include <SOP/SOP_Node.h>

namespace Neutron {
class SOP_Neutron : public SOP_Node
{
public:
	SOP_Neutron(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_Neutron();

    static CH_LocalVariable	 myVariables[];

    static PRM_Template		 myTemplateList[];
    static OP_Node		*myConstructor(OP_Network*, const char *,
							    OP_Operator *);

protected:

    /// Update disable and hidden states of parameters based on the value
    /// of other parameters.
//    virtual bool                 updateParmsFlags();
    //virtual const char          *inputLabel(unsigned idx) const;

    /// Method to cook geometry for the SOP
    virtual OP_ERROR		 cookMySop(OP_Context &context);
virtual OP_ERROR cookMyGuide1(OP_Context &context);


private:


	fpreal last = 0.0f;
};
} // End HDK_Sample namespace

#endif
