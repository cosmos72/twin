



/* This file was automatically generated from m4/method2.m4, do not edit! */
/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */


/*
 *  method2_m4.h  --  implementation of TT*_ttmethod functions,
 *                    TTCall*_ttmethod dispatcher (also used by ttlisteners)
 *
 */

/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */























ttbyte TTCallY_ttmethod(tt_obj id, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value, ttopaque nargs, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttany *a) {
    ttmethod m;
    ttopaque m_order;
    ttany a0 = (ttany)0;

    if ((m = ID2(ttmethod,id)) && nargs >= m->parameter_type->array_n) switch ((m_order = (opaque)id & TT_MAGIC_MASK)) {
    
	/* generic functions */




      case ttmethod_LibraryVersion:
	a0 = TTLibraryVersion();
	break;
      case ttmethod_ExitMainLoop:
	TTExitMainLoop();
	break;
      case ttmethod_New:
	a0 = (opaque)TTNew((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Del:
	TTDel((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_ClassOf:
	a0 = (opaque)TTClassOf((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_SuperClassOf:
	a0 = (opaque)TTSuperClassOf((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_InstanceOf:
	a0 = TTInstanceOf((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_ClassNameOf:
	a0 = (opaque)TTClassNameOf((tt_obj)(opaque)a[0]);
	break;








 
	/* ttobj methods */
                        
      case ttmethod_GetField_ttobj:
	a0 = TTGetField_ttobj((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *)(opaque)a[2]);
	break;
      case ttmethod_SetField_ttobj:
	a0 = TTSetField_ttobj((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (TT_ARG_READ TT_ARG_ARRAY((1)) ttarg *)(opaque)a[2]);
	break;
      case ttmethod_ChangeField_ttobj:
	a0 = TTChangeField_ttobj((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttany)a[2], (ttany)a[3]);
	break;
 
	/* ttclass methods */
    
      case ttmethod_GetName_ttclass:
	a0 = (opaque)TTGetName_ttclass((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_GetSize_ttclass:
	a0 = TTGetSize_ttclass((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_GetSuper_ttclass:
	a0 = (opaque)TTGetSuper_ttclass((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_IsInstance_ttclass:
	a0 = TTIsInstance_ttclass((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_FromType_ttclass:
	a0 = (opaque)TTFromType_ttclass((ttopaque)a[0]);
	break;
      case ttmethod_IsArrayType_ttclass:
	a0 = TTIsArrayType_ttclass((ttopaque)a[0]);
	break;
      case ttmethod_IsPrimitiveType_ttclass:
	a0 = TTIsPrimitiveType_ttclass((ttopaque)a[0]);
	break;
 
	/* ttobject methods */
                    
      case ttmethod_Ref_ttobject:
	TTRef_ttobject((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Unref_ttobject:
	TTUnref_ttobject((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_SetUserData_ttobject:
	a0 = TTSetUserData_ttobject((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
 
	/* ttvector methods */
        
      case ttmethod_GetAt_ttvector:
	a0 = TTGetAt_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1]);
	break;    
      case ttmethod_SetAt_ttvector:
	a0 = TTSetAt_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttany)a[2]);
	break;
      case ttmethod_Create1_ttvector:
	a0 = (opaque)TTCreate1_ttvector((ttany)a[0]);
	break;
      case ttmethod_CreateA_ttvector:
	a0 = (opaque)TTCreateA_ttvector((ttopaque)a[0], (TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttarg *)(opaque)a[1]);
	break;
      case ttmethod_CreateR_ttvector:
	a0 = (opaque)TTCreateR_ttvector((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_CreateV_ttvector:
	a0 = (opaque)TTCreateV_ttvector((ttopaque)a[0], (TT_ARG_ARRAY((1)) va_list *)(opaque)a[1]);
	break;
      case ttmethod_CreateY_ttvector:
	a0 = (opaque)TTCreateY_ttvector((ttopaque)a[0], (TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttany *)(opaque)a[1]);
	break;
      case ttmethod_Append_ttvector:
	a0 = TTAppend_ttvector((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case ttmethod_Add_ttvector:
	a0 = TTAdd_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttany)a[2]);
	break;
      case ttmethod_AddA_ttvector:
	a0 = TTAddA_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttopaque)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg *)(opaque)a[3]);
	break;
      case ttmethod_AddR_ttvector:
	a0 = TTAddR_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (tt_obj)(opaque)a[2]);
	break;
      case ttmethod_AddV_ttvector:
	a0 = TTAddV_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttopaque)a[2], (TT_ARG_ARRAY((1)) va_list *)(opaque)a[3]);
	break;    
      case ttmethod_AddY_ttvector:
	a0 = TTAddY_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttopaque)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttany *)(opaque)a[3]);
	break;    
      case ttmethod_ContainsValue_ttvector:
	a0 = TTContainsValue_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttany)a[2]);
	break;
      case ttmethod_RemoveAt_ttvector:
	a0 = TTRemoveAt_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1]);
	break;    
      case ttmethod_RemoveRange_ttvector:
	a0 = TTRemoveRange_ttvector((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttopaque)a[2]);
	break;
 
	/* ttfield methods */
    
      case ttmethod_Get_ttfield:
	a0 = (opaque)TTGet_ttfield((TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[0]);
	break;
      case ttmethod_GetValue_ttfield:
	a0 = TTGetValue_ttfield((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *)(opaque)a[2]);
	break;
      case ttmethod_SetValue_ttfield:
	a0 = TTSetValue_ttfield((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (TT_ARG_READ TT_ARG_ARRAY((1)) ttarg *)(opaque)a[2]);
	break;
      case ttmethod_ChangeValue_ttfield:
	a0 = TTChangeValue_ttfield((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttany)a[2], (ttany)a[3]);
	break;
      case ttmethod_ToEvcode_ttfield:
	a0 = TTToEvcode_ttfield((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_FromEvcode_ttfield:
	a0 = (opaque)TTFromEvcode_ttfield((ttopaque)a[0]);
	break;
      case ttmethod_GetEvcode_ttfield:
	a0 = TTGetEvcode_ttfield((TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[0]);
	break;
 
	/* ttmethod methods */
    
      case ttmethod_Get_ttmethod:
	a0 = (opaque)TTGet_ttmethod((TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[0]);
	break;
      case ttmethod_GetByAddress_ttmethod:
	a0 = (opaque)TTGetByAddress_ttmethod((TT_ARG_READ ttfunction_fn)(opaque)a[0]);
	break;
      case ttmethod_GetArraySizeA_ttmethod:
	a0 = TTGetArraySizeA_ttmethod((tt_obj)(opaque)a[0], (ttopaque)a[1], (ttopaque)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg *)(opaque)a[3]);
	break;
      case ttmethod_CheckArraySizesA_ttmethod:
	a0 = TTCheckArraySizesA_ttmethod((tt_obj)(opaque)a[0], (ttopaque)a[1], (TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg *)(opaque)a[2]);
	break;
      case ttmethod_CallA_ttmethod:
	a0 = TTCallA_ttmethod((tt_obj)(opaque)a[0], (TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *)(opaque)a[1], (ttopaque)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg *)(opaque)a[3]);
	break;
      case ttmethod_CallR_ttmethod:
	a0 = TTCallR_ttmethod((tt_obj)(opaque)a[0], (TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *)(opaque)a[1], (tt_obj)(opaque)a[2]);
	break;
      case ttmethod_CallV_ttmethod:
	a0 = TTCallV_ttmethod((tt_obj)(opaque)a[0], (TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *)(opaque)a[1], (ttopaque)a[2], (TT_ARG_ARRAY((1)) va_list *)(opaque)a[3]);
	break;
      case ttmethod_CallY_ttmethod:
	a0 = TTCallY_ttmethod((tt_obj)(opaque)a[0], (TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *)(opaque)a[1], (ttopaque)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttany *)(opaque)a[3]);
	break;
 
	/* ttevent methods */
    
      case ttmethod_Create_ttevent:
	a0 = (opaque)TTCreate_ttevent((ttuint)a[0], (ttuint)a[1], (ttuint)a[2]);
	break;
      case ttmethod_Fire_ttevent:
	TTFire_ttevent((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
	/* tteventbig methods */
    
      case ttmethod_Create8_tteventbig:
	a0 = (opaque)TTCreate8_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (ttshort)a[6], (ttany)a[7], (ttany)a[8], (ttopaque)a[9], (TT_ARG_READ TT_ARG_ARRAY((_P(10))) ttbyte *)(opaque)a[10]);
	break;
      case ttmethod_Create4_tteventbig:
	a0 = (opaque)TTCreate4_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (ttshort)a[6]);
	break;
      case ttmethod_Create4s4_tteventbig:
	a0 = (opaque)TTCreate4s4_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttany)a[3], (ttany)a[4], (ttopaque)a[5], (TT_ARG_READ TT_ARG_ARRAY((_P(6))) ttbyte *)(opaque)a[6]);
	break;
 
	/* ttbitmask methods */
    
      case ttmethod_Create_ttbitmask:
	a0 = (opaque)TTCreate_ttbitmask((ttany)a[0]);
	break;
      case ttmethod_CreateB2_ttbitmask:
	a0 = (opaque)TTCreateB2_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_CreateR_ttbitmask:
	a0 = (opaque)TTCreateR_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_CreateR2_ttbitmask:
	a0 = (opaque)TTCreateR2_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Clear_ttbitmask:
	TTClear_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Fill_ttbitmask:
	TTFill_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Negate_ttbitmask:
	TTNegate_ttbitmask((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Change_ttbitmask:
	TTChange_ttbitmask((tt_obj)(opaque)a[0], (ttuint)a[1]);
	break;
      case ttmethod_Contains_ttbitmask:
	a0 = TTContains_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case ttmethod_Add_ttbitmask:
	a0 = TTAdd_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case ttmethod_AddB_ttbitmask:
	a0 = TTAddB_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_AddR_ttbitmask:
	a0 = TTAddR_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_Remove_ttbitmask:
	a0 = TTRemove_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
      case ttmethod_RemoveB_ttbitmask:
	a0 = TTRemoveB_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_RemoveR_ttbitmask:
	a0 = TTRemoveR_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_Combine_ttbitmask:
	a0 = TTCombine_ttbitmask((tt_obj)(opaque)a[0], (ttany)a[1], (ttuint)a[2]);
	break;
      case ttmethod_CombineR_ttbitmask:
	a0 = TTCombineR_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2]);
	break;
      case ttmethod_CombineB_ttbitmask:
	a0 = TTCombineB_ttbitmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2]);
	break;
 
	/* tteventmask methods */
    
      case ttmethod_CreateB_tteventmask:
	a0 = (opaque)TTCreateB_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (tt_obj)(opaque)a[2]);
	break;
      case ttmethod_CreateB2_tteventmask:
	a0 = (opaque)TTCreateB2_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (tt_obj)(opaque)a[2]);
	break;
      case ttmethod_SetEvtypeMask_tteventmask:
	a0 = TTSetEvtypeMask_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_SetEvcodeMask_tteventmask:
	a0 = TTSetEvcodeMask_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_SetComponentMask_tteventmask:
	a0 = TTSetComponentMask_tteventmask((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_SetTruthTable_tteventmask:
	a0 = TTSetTruthTable_tteventmask((tt_obj)(opaque)a[0], (ttuint)a[1]);
	break;
 
	/* ttcallback methods */
        
 
	/* ttlistener methods */
    
      case ttmethod_Create_ttlistener:
	a0 = (opaque)TTCreate_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (TT_ARG_READ ttlistener_fn)(opaque)a[3], (ttany)a[4]);
	break;
      case ttmethod_CreateE_ttlistener:
	a0 = (opaque)TTCreateE_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (TT_ARG_READ ttlistener_fn)(opaque)a[3], (ttany)a[4]);
	break;
      case ttmethod_CreateR_ttlistener:
	a0 = (opaque)TTCreateR_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (TT_ARG_READ ttlistener_fn)(opaque)a[3], (ttopaque)a[4], (ttopaque)a[5], (tt_obj)(opaque)a[6]);
	break;
      case ttmethod_CreateDel_ttlistener:
	a0 = (opaque)TTCreateDel_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (TT_ARG_READ ttlistener_fn)(opaque)a[2]);
	break;
      case ttmethod_CreateAskclose_ttlistener:
	a0 = (opaque)TTCreateAskclose_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (TT_ARG_READ ttlistener_fn)(opaque)a[2]);
	break;
      case ttmethod_CreateKey_ttlistener:
	a0 = (opaque)TTCreateKey_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttuint)a[3], (TT_ARG_READ ttlistener_fn)(opaque)a[4]);
	break;
      case ttmethod_CreateMouse_ttlistener:
	a0 = (opaque)TTCreateMouse_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttuint)a[3], (TT_ARG_READ ttlistener_fn)(opaque)a[4]);
	break;
      case ttmethod_CreateActivate_ttlistener:
	a0 = (opaque)TTCreateActivate_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (TT_ARG_READ ttlistener_fn)(opaque)a[2]);
	break;
      case ttmethod_CreateChange_ttlistener:
	a0 = (opaque)TTCreateChange_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (TT_ARG_READ ttlistener_fn)(opaque)a[3]);
	break;
      case ttmethod_CreateKeyData_ttlistener:
	a0 = (opaque)TTCreateKeyData_ttlistener((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1], (ttuint)a[2], (TT_ARG_READ ttlistener_fn)(opaque)a[3]);
	break;
      case ttmethod_CreateExpose_ttlistener:
	a0 = (opaque)TTCreateExpose_ttlistener((tt_obj)(opaque)a[0], (ttuint)a[1], (TT_ARG_READ ttlistener_fn)(opaque)a[2]);
	break;    
      case ttmethod_AddTo_ttlistener:
	TTAddTo_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case ttmethod_Remove_ttlistener:
	TTRemove_ttlistener((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_SetArgsR_ttlistener:
	a0 = TTSetArgsR_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_SetEventMask_ttlistener:
	a0 = TTSetEventMask_ttlistener((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
	/* tttimer methods */
    
      case ttmethod_CreateR_tttimer:
	a0 = (opaque)TTCreateR_tttimer((tt_obj)(opaque)a[0], (ttuint)a[1], (TT_ARG_READ ttlistener_fn)(opaque)a[2], (ttopaque)a[3], (ttopaque)a[4], (tt_obj)(opaque)a[5], (ttany)a[6], (ttany)a[7]);
	break;
      case ttmethod_Create_tttimer:
	a0 = (opaque)TTCreate_tttimer((tt_obj)(opaque)a[0], (ttuint)a[1], (TT_ARG_READ ttlistener_fn)(opaque)a[2], (ttany)a[3], (ttany)a[4], (ttany)a[5]);
	break;
      case ttmethod_SetDelay_tttimer:
	TTSetDelay_tttimer((tt_obj)(opaque)a[0], (ttany)a[1], (ttany)a[2]);
	break;    
      case ttmethod_SetEnabled_tttimer:
	TTSetEnabled_tttimer((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case ttmethod_IsEnabled_tttimer:
	a0 = TTIsEnabled_tttimer((tt_obj)(opaque)a[0]);
	break;
 
	/* ttcomponent methods */
    
      case ttmethod_AddListener_ttcomponent:
	TTAddListener_ttcomponent((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_RemoveListener_ttcomponent:
	TTRemoveListener_ttcomponent((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_SetKeyData_ttcomponent:
	a0 = (opaque)TTSetKeyData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1], (ttany)a[2]);
	break;
      case ttmethod_GetKeyData_ttcomponent:
	a0 = TTGetKeyData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1]);
	break;
      case ttmethod_UnsetKeyData_ttcomponent:
	TTUnsetKeyData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1]);
	break;
      case ttmethod_GetData_ttcomponent:
	a0 = (opaque)TTGetData_ttcomponent((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1]);
	break;
 
	/* ttdata methods */
            
      case ttmethod_SetData_ttdata:
	a0 = TTSetData_ttdata((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;
 
	/* ttvisible methods */
        
      case ttmethod_AddTo_ttvisible:
	TTAddTo_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttany)a[2]);
	break;    
      case ttmethod_Validate_ttvisible:
	TTValidate_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case ttmethod_Remove_ttvisible:
	TTRemove_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case ttmethod_Invalidate_ttvisible:
	TTInvalidate_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case ttmethod_SetVisible_ttvisible:
	TTSetVisible_ttvisible((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case ttmethod_SetTheme_ttvisible:
	a0 = TTSetTheme_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case ttmethod_Draw_ttvisible:
	TTDraw_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte *)(opaque)a[6], (TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont *)(opaque)a[7], (TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr *)(opaque)a[8]);
	break;    
      case ttmethod_BuiltinRepaint_ttvisible:
	TTBuiltinRepaint_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;

      case ttmethod_SetRepaint_ttvisible:
	a0 = TTSetRepaint_ttvisible((tt_obj)(opaque)a[0], (TT_ARG_READ ttvisible_repaint_fn)(opaque)a[1]);
	break;
      case ttmethod_Expose_ttvisible:
	TTExpose_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
      case ttmethod_Add_ttvisible:
	TTAdd_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttany)a[2]);
	break;
 
	/* ttlayout methods */
            
 
	/* ttboxlayout methods */
    
      case ttmethod_Create_ttboxlayout:
	a0 = (opaque)TTCreate_ttboxlayout((ttany)a[0]);
	break;
 
	/* ttborderlayout methods */
    
 
	/* ttnative methods */
        
      case ttmethod_GetRoot_ttnative:
	a0 = (opaque)TTGetRoot_ttnative();
	break;    
      case ttmethod_GetW_ttnative:
	a0 = TTGetW_ttnative((tt_obj)(opaque)a[0]);
	break;    
      case ttmethod_GetH_ttnative:
	a0 = TTGetH_ttnative((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Create_ttnative:
	a0 = (opaque)TTCreate_ttnative((ttany)a[0]);
	break;
 
	/* ttwidget methods */
        
      case ttmethod_SetLayout_ttwidget:
	TTSetLayout_ttwidget((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case ttmethod_SetXY_ttwidget:
	a0 = TTSetXY_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2]);
	break;    
      case ttmethod_SetWH_ttwidget:
	a0 = TTSetWH_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2]);
	break;    
      case ttmethod_SetXl_ttwidget:
	a0 = TTSetXl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetYl_ttwidget:
	a0 = TTSetYl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetWl_ttwidget:
	a0 = TTSetWl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetHl_ttwidget:
	a0 = TTSetHl_ttwidget((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetTooltip_ttwidget:
	a0 = TTSetTooltip_ttwidget((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_SetXlYl_ttwidget:
	a0 = TTSetXlYl_ttwidget((tt_obj)(opaque)a[0], (ttuint)a[1], (ttint)a[2], (ttint)a[3]);
	break;
      case ttmethod_SetWlHl_ttwidget:
	a0 = TTSetWlHl_ttwidget((tt_obj)(opaque)a[0], (ttuint)a[1], (ttint)a[2], (ttint)a[3]);
	break;
      case ttmethod_SetXYWH_ttwidget:
	a0 = TTSetXYWH_ttwidget((tt_obj)(opaque)a[0], (ttuint)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5]);
	break;
      case ttmethod_SetX_ttwidget:
	a0 = TTSetX_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
      case ttmethod_SetY_ttwidget:
	a0 = TTSetY_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
      case ttmethod_SetW_ttwidget:
	a0 = TTSetW_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
      case ttmethod_SetH_ttwidget:
	a0 = TTSetH_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1]);
	break;
 
	/* ttlabel methods */
        
      case ttmethod_SetFontD_ttlabel:
	TTSetFontD_ttlabel((tt_obj)(opaque)a[0], (ttopaque)a[1], (TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont *)(opaque)a[2]);
	break;
      case ttmethod_SetFont_ttlabel:
	a0 = TTSetFont_ttlabel((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttfont *)(opaque)a[1]);
	break;
      case ttmethod_SetText_ttlabel:
	a0 = TTSetText_ttlabel((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1]);
	break;
      case ttmethod_Create_ttlabel:
	a0 = (opaque)TTCreate_ttlabel((TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[0]);
	break;
 
	/* tttooltip methods */
        
      case ttmethod_SetWidget_tttooltip:
	a0 = TTSetWidget_tttooltip((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
	/* ttbuttongroup methods */
        
      case ttmethod_Add_ttbuttongroup:
	TTAdd_ttbuttongroup((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case ttmethod_Remove_ttbuttongroup:
	TTRemove_ttbuttongroup((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case ttmethod_SetChecked_ttbuttongroup:
	TTSetChecked_ttbuttongroup((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
	/* ttanybutton methods */
            
      case ttmethod_SetAttrD_ttanybutton:
	TTSetAttrD_ttanybutton((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr *)(opaque)a[4]);
	break;
      case ttmethod_SetAttr_ttanybutton:
	a0 = TTSetAttr_ttanybutton((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr *)(opaque)a[4]);
	break;
      case ttmethod_SetFont_ttanybutton:
	a0 = TTSetFont_ttanybutton((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttfont *)(opaque)a[4]);
	break;
      case ttmethod_SetText_ttanybutton:
	a0 = TTSetText_ttanybutton((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttbyte *)(opaque)a[4]);
	break;
      case ttmethod_Create_ttanybutton:
	a0 = (opaque)TTCreate_ttanybutton((ttshort)a[0], (ttshort)a[1], (ttshort)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte *)(opaque)a[3]);
	break;
 
	/* ttbutton methods */
        
      case ttmethod_SetPressed_ttbutton:
	TTSetPressed_ttbutton((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;
      case ttmethod_IsPressed_ttbutton:
	a0 = TTIsPressed_ttbutton((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Create_ttbutton:
	a0 = (opaque)TTCreate_ttbutton((ttshort)a[0], (ttshort)a[1], (ttshort)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte *)(opaque)a[3]);
	break;
 
	/* ttcheckbutton methods */
        
      case ttmethod_SetChecked_ttcheckbutton:
	TTSetChecked_ttcheckbutton((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;
      case ttmethod_IsChecked_ttcheckbutton:
	a0 = TTIsChecked_ttcheckbutton((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Create_ttcheckbutton:
	a0 = (opaque)TTCreate_ttcheckbutton((ttshort)a[0], (ttshort)a[1], (ttshort)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte *)(opaque)a[3]);
	break;
 
	/* ttradiobutton methods */
    
      case ttmethod_AddToGroup_ttradiobutton:
	TTAddToGroup_ttradiobutton((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_RemoveFromGroup_ttradiobutton:
	TTRemoveFromGroup_ttradiobutton((tt_obj)(opaque)a[0]);
	break;
      case ttmethod_Create_ttradiobutton:
	a0 = (opaque)TTCreate_ttradiobutton((ttshort)a[0], (ttshort)a[1], (ttshort)a[2], (TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte *)(opaque)a[3]);
	break;
 
	/* ttanyscroll methods */
                
      case ttmethod_SetOrientation_ttanyscroll:
	a0 = TTSetOrientation_ttanyscroll((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case ttmethod_SetSize_ttanyscroll:
	a0 = TTSetSize_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetRealSize_ttanyscroll:
	a0 = TTSetRealSize_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetViewSize_ttanyscroll:
	a0 = TTSetViewSize_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetPosition_ttanyscroll:
	a0 = TTSetPosition_ttanyscroll((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;
 
	/* ttscrollbar methods */
    
      case ttmethod_Create1_ttscrollbar:
	a0 = (opaque)TTCreate1_ttscrollbar((ttbyte)a[0]);
	break;
      case ttmethod_Create4_ttscrollbar:
	a0 = (opaque)TTCreate4_ttscrollbar((ttbyte)a[0], (ttint)a[1], (ttint)a[2], (ttint)a[3]);
	break;
 
	/* ttslider methods */
        
      case ttmethod_SetSlideMin_ttslider:
	a0 = TTSetSlideMin_ttslider((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetSlideMax_ttslider:
	a0 = TTSetSlideMax_ttslider((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;    
      case ttmethod_SetSlideValue_ttslider:
	a0 = TTSetSlideValue_ttslider((tt_obj)(opaque)a[0], (ttint)a[1]);
	break;
      case ttmethod_Create_ttslider:
	a0 = (opaque)TTCreate_ttslider((ttbyte)a[0], (ttint)a[1]);
	break;
 
	/* ttprogressbar methods */
    
      case ttmethod_Create_ttprogressbar:
	a0 = (opaque)TTCreate_ttprogressbar((ttbyte)a[0]);
	break;
 
	/* ttscrollpane methods */
    
      case ttmethod_Create_ttscrollpane:
	a0 = (opaque)TTCreate_ttscrollpane((ttshort)a[0], (ttshort)a[1]);
	break;    
      case ttmethod_SetBarX_ttscrollpane:
	a0 = TTSetBarX_ttscrollpane((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case ttmethod_SetBarY_ttscrollpane:
	a0 = TTSetBarY_ttscrollpane((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
 
	/* ttwindow methods */
        
      case ttmethod_SetTitle_ttwindow:
	a0 = TTSetTitle_ttwindow((tt_obj)(opaque)a[0], (TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[1]);
	break;
 
	/* ttframe methods */
        
      case ttmethod_SetMenubar_ttframe:
	a0 = TTSetMenubar_ttframe((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;
      case ttmethod_Create_ttframe:
	a0 = (opaque)TTCreate_ttframe((TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[0]);
	break;
 
	/* ttscroller methods */
    
      case ttmethod_Create_ttscroller:
	a0 = (opaque)TTCreate_ttscroller((ttshort)a[0], (ttshort)a[1]);
	break;
 
	/* ttmenuitem methods */
    
 
	/* ttcheckmenuitem methods */
    
 
	/* ttradiomenuitem methods */
    
 
	/* ttmenu methods */
    
 
	/* ttmenubar methods */
    
      case ttmethod_GetDefault_ttmenubar:
	a0 = (opaque)TTGetDefault_ttmenubar();
	break;
 
	/* ttanytext methods */
    
 
	/* tttextfield methods */
    
 
	/* tttextarea methods */
    
 
	/* tttheme methods */
    
      case ttmethod_GetDefault_tttheme:
	a0 = (opaque)TTGetDefault_tttheme();
	break;
 
	/* ttapplication methods */
    
      case ttmethod_Create_ttapplication:
	a0 = (opaque)TTCreate_ttapplication((TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *)(opaque)a[0]);
	break;
      case ttmethod_Get_ttapplication:
	a0 = (opaque)TTGet_ttapplication();
	break;






	/* TTSet*_* methods omitted, they are listed above ! */
    
	/* TTGet*_* methods */

      default:
        if (m_order >= ttmethod_FN_get_first && m_order < ttmethod_FN_get_last) {
	  return TTGetField_ttobj((tt_obj)(opaque)a[1],
				  (tt_obj)(opaque)((order_ttfield<<TT_MAGIC_SHIFT) |
						   (m_order - ttmethod_FN_get_first +
						    ttobj_field_first)),
				  return_value);
	}
	return TT_FALSE;
	
    } else
	return TT_FALSE;
	
    return_value->type = m->return_type;
    return_value->value = a0;
    return TT_TRUE;
}

