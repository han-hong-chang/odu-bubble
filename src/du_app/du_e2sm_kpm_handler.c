#include "du_e2sm_kpm_handler.h"
#include "S-NSSAI.h"
#include "SD.h"

/*******************************************************************
 *
 * @brief Initialize the E2SM-KPM module
 *
 * @details
 *
 *    Function : kpmInit
 *
 *    Functionality: Initialize the parameters in the KPM module
 *
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmInit(){
    uint8_t cellMeasName[][25] = {"DRB.UEThpDl", "RRU.PrbUsedDl", "RRU.PrbAvailDl", "RRU.PrbTotDl"}; // UE throughput, Avaliable PRB, Total PRB Usage
    uint8_t sliceMeasName[][25] = {"DRB.UEThpDl.SNSSAI", "RRU.PrbUsedDl.SNSSAI"}; // UE throughput, Used PRB, Total PRB

    kpmCellIndicationEnable = false;
    kpmSliceIndicationEnable = false;
    reportingPeriodFmt1 = 0;
    reportingPeriodFmt2 = 0;
    kpmIndicationV3Enable = 0;
    reportingPeriod = 0;
    kpiModuleInit();
    
    for(int i=0;i<SUPPORTED_CELL_MEAS_ITEM;i++){
         memcpy(cellMetricName[i], cellMeasName[i], MAX_OF_MEAS_NAME);
    }
    
    for(int i=0;i<SUPPORTED_SLICE_MEAS_ITEM;i++){
         memcpy(sliceMetricName[i], sliceMeasName[i], MAX_OF_MEAS_NAME);
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Enable the Indication flag
 *
 * @details
 *
 *    Function : kpmEnableIndication
 *
 *    Functionality: Enable the Indication flag
 *
 * @param[in] flag, bool* for indication flag
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmEnableIndication(bool *flag){
    if(flag){
        *flag = true;
        return ROK;
    }else{
        DU_LOG("\nERROR  --> E2SM-KPM: Failed to enable the flag, %d",__LINE__);
        return RFAILED;
    } 
}

/*******************************************************************
 *
 * @brief Fill in the Octet String
 *
 * @details
 *
 *    Function : kpmFillOctetString
 *
 *    Functionality: copy the memory from uint8_t src to Octet_String_t dst
 *
 * @param[in] dst, Octet_String_t* for destination
 * @param[in] src, uint8_t* for source
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillOctetString(OCTET_STRING_t *dst, uint8_t *src){
    dst->size = strlen((const char*)src);
    DU_ALLOC(dst->buf, dst->size);
    if(dst->buf == NULL){
        DU_LOG("\nERROR  --> E2SM-KPM: Memory allocation failed for kpmFillOctetString %d",__LINE__);
        return RFAILED;
    }
    memcpy(dst->buf, src, dst->size);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the Printable String
 *
 * @details
 *
 *    Function : kpmFillPrintableString
 *
 *    Functionality: copy the memory from uint8_t src to Octet_String_t dst. Fill '\0' at the end. 
 *
 * @param[in] dst, PrintableString_t* for destination
 * @param[in] src, uint8_t* for source
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillPrintableString(PrintableString_t *dst, uint8_t *src){
    dst->size = strlen((const char*)src) + 1;
    DU_ALLOC(dst->buf, dst->size);
    if(dst->buf == NULL){
        DU_LOG("\nERROR  --> E2SM-KPM: Memory allocation failed for kpmFillOctetString %d",__LINE__);
        return RFAILED;
    }
    memcpy(dst->buf, src, dst->size);
    dst->buf[dst->size - 1] = 0;
    return ROK;
}


/*******************************************************************
 *
 * @brief Fill in the RAN Function Name
 *
 * @details
 *
 *    Function : kpmFillRanFunctionName
 *
 *    Functionality: Fill in the RAN Function Name for RAN Function Description
 *
 * @param[in] ranfunc_name, RANFunction_Name_t* for input source
 * @param[in] kpmVersion, uint8_t for indicating the E2SM-KPM version
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRanFunctionName(RANfunction_Name_t *ranfunc_name, uint8_t kpmVersion){
    uint8_t ret = ROK;
    uint8_t ranfunc_defin[] = "ORAN-E2SM-KPM"; // RAN Function Definition,
    uint8_t ranfunc_descr[] = "KPM monitor"; // RAN function description
    uint8_t ranfunc_oidv2[] = "1.3.6.1.4.1.53148.1.2.2.2"; // RAN function service model OIDv2
    uint8_t ranfunc_oidv3[] = "1.3.6.1.4.1.53148.1.3.2.2"; // RAN function service model OIDv3

    ret = kpmFillOctetString(&ranfunc_name->ranFunction_ShortName, ranfunc_defin);
    if(ret != ROK){
        DU_LOG("\nERROR  --> E2SM-KPM: Failed to assign RAN Function short name, %d",__LINE__);
        return RFAILED;
    }
    ret = kpmFillOctetString(&ranfunc_name->ranFunction_Description, ranfunc_descr);
    if(ret != ROK){
        DU_LOG("\nERROR  --> E2SM-KPM: Failed to assign RAN Function description, %d",__LINE__);
        return RFAILED;
    }

    if(kpmVersion == 2){
        ret = kpmFillOctetString(&ranfunc_name->ranFunction_E2SM_OID, ranfunc_oidv2);
        if(ret != ROK){
            DU_LOG("\nERROR  --> E2SM-KPM: Failed to assign RAN Function service model OID v2, %d",__LINE__);
            return RFAILED;
        }
    }
    else if(kpmVersion == 3){
        ret = kpmFillOctetString(&ranfunc_name->ranFunction_E2SM_OID, ranfunc_oidv3);
        if(ret != ROK){
            DU_LOG("\nERROR  --> E2SM-KPM: Failed to assign RAN Function service model OID v3, %d",__LINE__);
            return RFAILED;
        }
    }else{
        DU_LOG("\nERROR   -->  E2SM-KPM : Invalid KPM version");
        return RFAILED;
    }

    DU_ALLOC(ranfunc_name->ranFunction_Instance, sizeof(long));
    if(ranfunc_name->ranFunction_Instance == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN function instance failed");
        return RFAILED;
    }
    *ranfunc_name->ranFunction_Instance = 1;
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in trigger style item
 *
 * @details
 *
 *    Function : kpmFillTriggerStyleItem
 *
 *    Functionality: Fill in the trigger style item for RAN Function Description
 *
 * @param[in] trigger_style, RIC_EventTriggerStyle_Item_t* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillTriggerStyleItem(RIC_EventTriggerStyle_Item_t *trigger_style){
    uint8_t trig_name[] = "Periodic report";
    uint8_t ret = ROK;
    trigger_style->ric_EventTriggerStyle_Type = 1; // KPM only supports type 1

    ret = kpmFillOctetString(&trigger_style->ric_EventTriggerStyle_Name, trig_name);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Event Trigger name, %d",__LINE__);
        return RFAILED;
    }
    trigger_style->ric_EventTriggerFormat_Type = 1;
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in trigger style list
 *
 * @details
 *
 *    Function : kpmFillTriggerStyleList
 *
 *    Functionality: Fill in the trigger style list for RAN Function Description
 *
 * @param[in] triggerStyleList, E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List* for source list
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillTriggerStyleList(struct E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List *triggerStyleList){
    uint8_t ret = ROK;
    RIC_EventTriggerStyle_Item_t *trigger_style;
    DU_ALLOC(trigger_style, sizeof(RIC_EventTriggerStyle_Item_t));
    if(trigger_style == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Event Trigger Style failed");
        return RFAILED;
    }
    ret = kpmFillTriggerStyleItem(trigger_style); // fill one trigger style
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Event Trigger Style, %d", __LINE__);
        return RFAILED;
    }
    ASN_SEQUENCE_ADD(&triggerStyleList->list, trigger_style);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in trigger style list in KPMv3
 *
 * @details
 *
 *    Function : kpmFillTriggerStyleListV3
 *
 *    Functionality: Fill in the trigger style item for RAN Function Description
 *
 * @param[in] triggerStyleList, E2SM_KPM_RANfunction_Description_v300__ric_EventTriggerStyle_List* for source list
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillTriggerStyleListV3(struct E2SM_KPM_RANfunction_Description_v300__ric_EventTriggerStyle_List *triggerStyleList){
    uint8_t ret = ROK;
    RIC_EventTriggerStyle_Item_t *trigger_style;
    DU_ALLOC(trigger_style, sizeof(RIC_EventTriggerStyle_Item_t));
    if(trigger_style == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Event Trigger Style failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillTriggerStyleItem(trigger_style); // fill one trigger style
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Event Trigger Style, %d", __LINE__);
        return RFAILED;
    }
    ASN_SEQUENCE_ADD(&triggerStyleList->list, trigger_style);
    
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in report style item in style 1
 *
 * @details
 *
 *    Function : kpmFillReportStyleItemStyle1
 *
 *    Functionality: Fill in the trigger style list for cell metrics
 *
 * @param[in] report_style, RIC_ReportStyle_Item_t* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillReportStyleItemStyle1(RIC_ReportStyle_Item_t *report_style){
    uint8_t report_name[] = "O-DU Cell Metrics for SLA Functionality";
    uint8_t ret = ROK;
    report_style->ric_ReportStyle_Type = 1; // For Report
    ret = kpmFillOctetString(&report_style->ric_ReportStyle_Name, report_name);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Report Style name of cell metrics, %d",__LINE__);
        return RFAILED;
    }
    report_style->ric_IndicationHeaderFormat_Type = 1; // only 1 supported in KPM 2.03
    report_style->ric_IndicationMessageFormat_Type = 1;
    report_style->ric_ActionFormat_Type = 1; // new in KPM 2.0
    MeasurementInfo_Action_Item_t *measure_item;
    DU_ALLOC(measure_item, SUPPORTED_CELL_MEAS_ITEM * sizeof(MeasurementInfo_Action_Item_t));
    if(measure_item == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for measurement info action item of cell metrics failed, %d", __LINE__);
        return RFAILED;
    }
    for(int i=0;i<SUPPORTED_CELL_MEAS_ITEM;i++){
        ret = kpmFillOctetString(&measure_item[i].measName, cellMetricName[i]);
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Measurement Info Action measurement name of cell metrics, %d",__LINE__);
            return RFAILED;
        }
        ASN_SEQUENCE_ADD(&report_style->measInfo_Action_List.list, measure_item + i);
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in report style item in style 1 of KPMv3
 *
 * @details
 *
 *    Function : kpmFillReportStyleItemStyle1V3
 *
 *    Functionality: Fill in the trigger style item for cell metrics
 *
 * @param[in] report_style, RIC_ReportStyle_Item_v300_t* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillReportStyleItemStyle1V3(RIC_ReportStyle_Item_v300_t *report_style){
    uint8_t ret = ROK;
    uint8_t report_name[] = "O-DU Cell Metrics for SLA Functionality";

    report_style->ric_ReportStyle_Type = 1; // For Report
    ret = kpmFillOctetString(&report_style->ric_ReportStyle_Name, report_name);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Report Style name of cell metrics, %d",__LINE__);
        return RFAILED;
    }

    report_style->ric_IndicationHeaderFormat_Type = 1; // only 1 supported in KPM 2.03
    report_style->ric_IndicationMessageFormat_Type = 1;
    report_style->ric_ActionFormat_Type = 1; // new in KPM 2.0

    MeasurementInfo_Action_Item_v300_t *measure_item;
    DU_ALLOC(measure_item, SUPPORTED_CELL_MEAS_ITEM * sizeof(MeasurementInfo_Action_Item_v300_t));
    if(measure_item == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for measurement info action item of cell metrics failed, %d", __LINE__);
        return RFAILED;
    }

    for(int i=0;i<SUPPORTED_CELL_MEAS_ITEM;i++){
        ret = kpmFillOctetString(&measure_item[i].measName, cellMetricName[i]);
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Measurement Info Action measurement name of cell metrics, %d",__LINE__);
            return RFAILED;
        }
        ASN_SEQUENCE_ADD(&report_style->measInfo_Action_List.list, measure_item + i);
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in report style item in style 2
 *
 * @details
 *
 *    Function : kpmFillReportStyleItemStyle2
 *
 *    Functionality: Fill in the trigger style item for slice metrics
 *
 * @param[in] report_style, RIC_ReportStyle_Item_t* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillReportStyleItemStyle2(RIC_ReportStyle_Item_t *report_style){
    uint8_t ret = ROK;
    uint8_t report_name[] = "O-DU Slice Metrics for SLA Functionality";

    ret = kpmFillOctetString(&report_style->ric_ReportStyle_Name, report_name);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Report Style name of slice metrics, %d",__LINE__);
        return RFAILED;
    }
    report_style->ric_ReportStyle_Type = 3; // For reporting slice measurement
    report_style->ric_IndicationHeaderFormat_Type = 1; // only 1 supported in KPM 2.03
    report_style->ric_IndicationMessageFormat_Type = 2; // For conditional reporting
    report_style->ric_ActionFormat_Type = 3; // new in KPM 2.0

    MeasurementInfo_Action_Item_t *measure_item;
    DU_ALLOC(measure_item, SUPPORTED_SLICE_MEAS_ITEM * sizeof(MeasurementInfo_Action_Item_t));
    if(measure_item == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for measurement info action item of slice metrics failed, %d", __LINE__);
        return RFAILED;
    }
    for(int i=0;i<SUPPORTED_SLICE_MEAS_ITEM;i++){
        ret = kpmFillOctetString(&measure_item[i].measName, sliceMetricName[i]);
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Measurement Info Action measurement name of slice metrics, %d",__LINE__);
            return RFAILED;
        }
        ASN_SEQUENCE_ADD(&report_style->measInfo_Action_List.list, measure_item + i);
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in report style item in style 2 of KPMv3
 *
 * @details
 *
 *    Function : kpmFillReportStyleItemStyle2V3
 *
 *    Functionality: Fill in the trigger style item for slice metrics
 *
 * @param[in] report_style, RIC_ReportStyle_Item_v300_t* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillReportStyleItemStyle2V3(RIC_ReportStyle_Item_v300_t *report_style){
    uint8_t ret = ROK;
    uint8_t report_name[] = "O-DU Slice Metrics for SLA Functionality";

    report_style->ric_IndicationHeaderFormat_Type = 1; // only 1 supported in KPM 2.03
    report_style->ric_IndicationMessageFormat_Type = 2; // For conditional reporting
    report_style->ric_ActionFormat_Type = 3; // new in KPM 2.0
    report_style->ric_ReportStyle_Type = 3; // For reporting slice measurement
    
    ret = kpmFillOctetString(&report_style->ric_ReportStyle_Name, report_name);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Report Style name of slice metrics, %d",__LINE__);
        return RFAILED;
    }

    MeasurementInfo_Action_Item_v300_t *measure_item;
    DU_ALLOC(measure_item, SUPPORTED_SLICE_MEAS_ITEM * sizeof(MeasurementInfo_Action_Item_v300_t));
    if(measure_item == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for measurement info action item of slice metrics failed, %d", __LINE__);
        return RFAILED;
    }

    for(int i=0;i<SUPPORTED_SLICE_MEAS_ITEM;i++){
        ret = kpmFillOctetString(&measure_item[i].measName, sliceMetricName[i]);
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Failed to assign Measurement Info Action measurement name of slice metrics, %d",__LINE__);
            return RFAILED;
        }
        ASN_SEQUENCE_ADD(&report_style->measInfo_Action_List.list, measure_item + i);
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in report style list
 *
 * @details
 *
 *    Function : kpmFillReportStyleList
 *
 *    Functionality: Fill in the trigger style list for cell/slice metrics
 *
 * @param[in] reportStyleList, E2SM_KPM_RANfunction_Description__ric_ReportStyle_List* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillReportStyleList(struct E2SM_KPM_RANfunction_Description__ric_ReportStyle_List *reportStyleList){
    uint8_t ret = ROK;
    RIC_ReportStyle_Item_t *report_style;
    DU_ALLOC(report_style, sizeof(RIC_ReportStyle_Item_t) * 2); // slice metrics and cell metrics
    if(report_style == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for report style item failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillReportStyleItemStyle1(report_style);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill report style item style 1, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillReportStyleItemStyle2(report_style+1);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill report style item style 2, %d", __LINE__);
        return RFAILED;
    }
    ASN_SEQUENCE_ADD(&reportStyleList->list, report_style);
    ASN_SEQUENCE_ADD(&reportStyleList->list, report_style+1);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in report style list of KPMv3
 *
 * @details
 *
 *    Function : kpmFillReportStyleListV3
 *
 *    Functionality: Fill in the trigger style list for cell/slice metrics
 *
 * @param[in] reportStyleList, E2SM_KPM_RANfunction_Description_v300__ric_ReportStyle_List* for source item
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillReportStyleListV3(struct E2SM_KPM_RANfunction_Description_v300__ric_ReportStyle_List *reportStyleList){
    uint8_t ret = ROK;
    RIC_ReportStyle_Item_v300_t *report_style;
    DU_ALLOC(report_style, sizeof(RIC_ReportStyle_Item_v300_t) * 2); // slice metrics and cell metrics
    if(report_style == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for report style item failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillReportStyleItemStyle1V3(report_style);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill report style item style 1, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillReportStyleItemStyle2V3(report_style+1);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill report style item style 2, %d", __LINE__);
        return RFAILED;
    }
    ASN_SEQUENCE_ADD(&reportStyleList->list, report_style);
    ASN_SEQUENCE_ADD(&reportStyleList->list, report_style+1);
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for RANfunction_Name_t 
 *
 * @details
 *
 *    Function : kpmFreeRanFuncName
 *
 *    Functionality: freeing the memory allocated for RANfunction_Name_t
 *
 * @param[in] ranfunc_name, RANfunction_Name_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeRanFuncName(RANfunction_Name_t *ranfunc_name){
    if(ranfunc_name->ranFunction_Description.size){
        DU_FREE(ranfunc_name->ranFunction_Description.buf, ranfunc_name->ranFunction_Description.size);
    }
    if(ranfunc_name->ranFunction_ShortName.size){
        DU_FREE(ranfunc_name->ranFunction_ShortName.buf, ranfunc_name->ranFunction_ShortName.size);
    }
    if(ranfunc_name->ranFunction_Description.size){
        DU_FREE(ranfunc_name->ranFunction_Description.buf, ranfunc_name->ranFunction_Description.size);
    }
    if(ranfunc_name->ranFunction_Instance){
        DU_FREE(ranfunc_name->ranFunction_Instance, sizeof(long));
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List 
 *
 * @details
 *
 *    Function : kpmFreeEventTrigStyleList
 *
 *    Functionality: freeing the memory allocated for E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List
 *
 * @param[in] eventTrigStyleList, E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeEventTrigStyleList(struct E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List *eventTrigStyleList){
    for(int i=0;i<eventTrigStyleList->list.count;i++){
        if(eventTrigStyleList->list.array[i]){
            if(eventTrigStyleList->list.array[i]->ric_EventTriggerStyle_Name.size>0){
                if(eventTrigStyleList->list.array[i]->ric_EventTriggerStyle_Name.buf){
                    DU_FREE(eventTrigStyleList->list.array[i]->ric_EventTriggerStyle_Name.buf, eventTrigStyleList->list.array[i]->ric_EventTriggerStyle_Name.size);
                }
            }
        }  
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_RANfunction_Description__ric_ReportStyle_List 
 *
 * @details
 *
 *    Function : kpmFreeReportStyleList
 *
 *    Functionality: freeing the memory allocated for E2SM_KPM_RANfunction_Description__ric_ReportStyle_List
 *
 * @param[in] reportStyleList, E2SM_KPM_RANfunction_Description__ric_ReportStyle_List*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeReportStyleList(struct E2SM_KPM_RANfunction_Description__ric_ReportStyle_List *reportStyleList){
    for(int i=0;i<reportStyleList->list.count;i++){
        if(reportStyleList->list.array[i]){
            if(reportStyleList->list.array[i]->ric_ReportStyle_Name.size>0){
                if(reportStyleList->list.array[i]->ric_ReportStyle_Name.buf){
                    DU_FREE(reportStyleList->list.array[i]->ric_ReportStyle_Name.buf, reportStyleList->list.array[i]->ric_ReportStyle_Name.size);
                }
            }
            for(int j=0;j<reportStyleList->list.array[i]->measInfo_Action_List.list.count;j++){
                if(reportStyleList->list.array[i]->measInfo_Action_List.list.array[j]->measName.size>0){
                    if(reportStyleList->list.array[i]->measInfo_Action_List.list.array[j]->measName.buf){
                        DU_FREE(reportStyleList->list.array[i]->measInfo_Action_List.list.array[j]->measName.buf, reportStyleList->list.array[i]->measInfo_Action_List.list.array[j]->measName.size);
                    }
                    if(reportStyleList->list.array[i]->measInfo_Action_List.list.array[j]->measID){
                        DU_FREE(reportStyleList->list.array[i]->measInfo_Action_List.list.array[j]->measID, sizeof(long));
                    }
                }
            }
        }
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_RANfunction_Description_t 
 *
 * @details
 *
 *    Function : kpmFreeRanFuncDesc
 *
 *    Functionality: freeing the memory allocated for RAN Function Description
 *
 * @param[in] ranfunc_desc, E2SM_KPM_RANfunction_Description_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeRanFuncDesc(E2SM_KPM_RANfunction_Description_t *ranfunc_desc){
    kpmFreeRanFuncName(&ranfunc_desc->ranFunction_Name);
    if(ranfunc_desc->ric_EventTriggerStyle_List){
        kpmFreeEventTrigStyleList(ranfunc_desc->ric_EventTriggerStyle_List);
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Build the RAN Function Description of KPMv3 
 *
 * @details
 *
 *    Function : kpmFillRanFuncDescriptionV3
 *
 *    Functionality: Fills the RAN Function Description of KPMv3
 *
 * @param[in] ranFunDefinition, RANfunctionDefinition_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRanFuncDescriptionV3(RANfunctionDefinition_t  *ranFunDefinition){
    asn_codec_ctx_t *opt_cod = 0;
    uint8_t ret = ROK;
    E2SM_KPM_RANfunction_Description_v300_t *ranfunc_desc;
    DU_ALLOC(ranfunc_desc, sizeof(E2SM_KPM_RANfunction_Description_v300_t));
    if(ranfunc_desc == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description failed, %d", __LINE__);
        return RFAILED;
    }

    ASN_STRUCT_RESET(asn_DEF_E2SM_KPM_RANfunction_Description_v300, ranfunc_desc);

    ret = kpmFillRanFunctionName(&ranfunc_desc->ranFunction_Name, 3);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill RAN Function Name, %d", __LINE__);
        return RFAILED;
    }

    DU_ALLOC(ranfunc_desc->ric_EventTriggerStyle_List, sizeof(struct E2SM_KPM_RANfunction_Description_v300__ric_EventTriggerStyle_List));
    if(ranfunc_desc->ric_EventTriggerStyle_List == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description - Event Trigger Style List failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(ranfunc_desc->ric_ReportStyle_List, sizeof(struct E2SM_KPM_RANfunction_Description_v300__ric_ReportStyle_List));
    if(ranfunc_desc->ric_ReportStyle_List == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description - Report Style List failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillTriggerStyleListV3(ranfunc_desc->ric_EventTriggerStyle_List);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Trigger Style List, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillReportStyleListV3(ranfunc_desc->ric_ReportStyle_List);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Report Style List, %d", __LINE__);
        return RFAILED;
    }

    xer_fprint(stderr, &asn_DEF_E2SM_KPM_RANfunction_Description_v300, ranfunc_desc);

    uint8_t e2smbuffer[8192];
    size_t e2smbuffer_size = 8192;
    asn_enc_rval_t     encRetVal;

    encRetVal = asn_encode_to_buffer(opt_cod,
         ATS_ALIGNED_BASIC_PER,
         &asn_DEF_E2SM_KPM_RANfunction_Description,
         ranfunc_desc, e2smbuffer, e2smbuffer_size);
    
    if(encRetVal.encoded == -1){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to encode RAN Function Description, %d", __LINE__);
        return RFAILED;
    }else{
        DU_ALLOC(ranFunDefinition->buf, encRetVal.encoded *  sizeof(uint8_t));
        if(ranFunDefinition->buf == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description failed, %d", __LINE__);
            return RFAILED;
        }
        ranFunDefinition->size = encRetVal.encoded;
        memcpy(ranFunDefinition->buf, e2smbuffer, encRetVal.encoded);
        return ROK;
    }    
}

/*******************************************************************
 *
 * @brief Build the RAN Function Description
 *
 * @details
 *
 *    Function : kpmFillRanFuncDescription
 *
 *    Functionality: Fills the RAN Function Description
 *
 * @param[in] ranFunDefinition, RANfunctionDefinition_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRanFuncDescription(RANfunctionDefinition_t  *ranFunDefinition){
    uint8_t ret = ROK;
    asn_codec_ctx_t *opt_cod = 0;
    E2SM_KPM_RANfunction_Description_t *ranfunc_desc;
    DU_ALLOC(ranfunc_desc, sizeof(E2SM_KPM_RANfunction_Description_t));
    if(ranfunc_desc == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description failed, %d", __LINE__);
        return RFAILED;
    }

    ASN_STRUCT_RESET(asn_DEF_E2SM_KPM_RANfunction_Description, ranfunc_desc);
    ret = kpmFillRanFunctionName(&ranfunc_desc->ranFunction_Name, 2);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill RAN Function Name, %d", __LINE__);
        return RFAILED;
    }

    DU_ALLOC(ranfunc_desc->ric_EventTriggerStyle_List, sizeof(struct E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List));
    if(ranfunc_desc->ric_EventTriggerStyle_List == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description - Event Trigger Style List failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(ranfunc_desc->ric_ReportStyle_List, sizeof(struct E2SM_KPM_RANfunction_Description__ric_ReportStyle_List));
    if(ranfunc_desc->ric_ReportStyle_List == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description - Report Style List failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillTriggerStyleList(ranfunc_desc->ric_EventTriggerStyle_List);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Trigger Style List, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillReportStyleList(ranfunc_desc->ric_ReportStyle_List);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Report Style List, %d", __LINE__);
        return RFAILED;
    }
    xer_fprint(stderr, &asn_DEF_E2SM_KPM_RANfunction_Description, ranfunc_desc);

    uint8_t e2smbuffer[8192];
    size_t e2smbuffer_size = 8192;
    asn_enc_rval_t     encRetVal;
    encRetVal = asn_encode_to_buffer(opt_cod,
         ATS_ALIGNED_BASIC_PER,
         &asn_DEF_E2SM_KPM_RANfunction_Description,
         ranfunc_desc, e2smbuffer, e2smbuffer_size);
    
    if(encRetVal.encoded == -1){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to encode RAN Function Description, %d", __LINE__);
        return RFAILED;
    }else{
        DU_ALLOC(ranFunDefinition->buf, encRetVal.encoded *  sizeof(uint8_t));
        if(ranFunDefinition->buf == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RAN Function Description failed, %d", __LINE__);
            return RFAILED;
        }
        ranFunDefinition->size = encRetVal.encoded;
        memcpy(ranFunDefinition->buf, e2smbuffer, encRetVal.encoded);
        return ROK;
    }
}

/*******************************************************************
 *
 * @brief Build the measurement info item
 *
 * @details
 *
 *    Function : kpmFillMeasInfoItem
 *
 *    Functionality: Fills the MeasurementInfoItem_t for indication message
 *
 * @param[in] measInfoItem, MeasurementInfoItem_t*, destination
 * @param[in] measName, measInfoItem*, the source of measurement name
 * @param[in] size, int, the size of the measurement name
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillMeasInfoItem(MeasurementInfoItem_t *measInfoItem, uint8_t *measName, int size){
    LabelInfoItem_t *measLabelInfo;
    DU_ALLOC(measLabelInfo, sizeof(LabelInfoItem_t));
    if(measLabelInfo == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Label Info item failed, %d", __LINE__);
        return RFAILED;
    }

    long *measLabelInfo_noLabel;
    DU_ALLOC(measLabelInfo_noLabel, sizeof(long));
    if(measLabelInfo_noLabel == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for No Label of Label Info Item failed, %d", __LINE__);
        return RFAILED;
    }
    measInfoItem->measType.present = MeasurementType_PR_measName; // Choose name or ID
    measInfoItem->measType.choice.measName.size = size;
    DU_ALLOC(measInfoItem->measType.choice.measName.buf, size);
    if(measInfoItem->measType.choice.measName.buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for measurement name of lLbel info item failed, %d", __LINE__);
        return RFAILED;
    }
    memcpy(measInfoItem->measType.choice.measName.buf, measName, measInfoItem->measType.choice.measName.size);
    *measLabelInfo_noLabel = MeasurementLabel__noLabel_true;
    measLabelInfo->measLabel.noLabel = measLabelInfo_noLabel;
    ASN_SEQUENCE_ADD(&measInfoItem->labelInfoList.list, measLabelInfo);
    return ROK;
}

/*******************************************************************
 *
 * @brief Build the measurement info item of KPMv3
 *
 * @details
 *
 *    Function : kpmFillMeasInfoItemV3
 *
 *    Functionality: Fills the MeasurementInfoItem_v300_t for indication message
 *
 * @param[in] measInfoItem, MeasurementInfoItem_v300_t*, destination
 * @param[in] measName, measInfoItem*, the source of measurement name
 * @param[in] size, int, the size of the measurement name
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillMeasInfoItemV3(MeasurementInfoItem_v300_t *measInfoItem, uint8_t *measName, int size){
    LabelInfoItem_v300_t *measLabelInfo;
    DU_ALLOC(measLabelInfo, sizeof(LabelInfoItem_v300_t));
    if(measLabelInfo == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Label Info Item failed, %d", __LINE__);
        return RFAILED;
    }

    long *measLabelInfo_noLabel;
    DU_ALLOC(measLabelInfo_noLabel, sizeof(long));
    if(measLabelInfo_noLabel == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for No Label of Label Info Item failed, %d", __LINE__);
        return RFAILED;
    }
    measInfoItem->measType.present = MeasurementType_PR_measName; // Choose name or ID
    measInfoItem->measType.choice.measName.size = size;
    DU_ALLOC(measInfoItem->measType.choice.measName.buf, size);
    if(measInfoItem->measType.choice.measName.buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for measurement name of lLbel info item failed, %d", __LINE__);
        return RFAILED;
    }
    memcpy(measInfoItem->measType.choice.measName.buf, measName, measInfoItem->measType.choice.measName.size);
    *measLabelInfo_noLabel = MeasurementLabel__noLabel_true;
    measLabelInfo->measLabel.noLabel = measLabelInfo_noLabel;
    ASN_SEQUENCE_ADD(&measInfoItem->labelInfoList.list, measLabelInfo);
    return ROK;
}

/*******************************************************************
 *
 * @brief Process the event trigger definition format 1
 *
 * @details
 *
 *    Function : kpmProcEventTrigDefinitionFormat1
 *
 *    Functionality: Process the RICeventTriggerDefinition_t and get reporting period
 *
 * @param[in] defini, RICeventTriggerDefinition_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcEventTrigDefinitionFormat1(RICeventTriggerDefinition_t *defini){
    asn_dec_rval_t  rval;
    E2SM_KPM_EventTriggerDefinition_t *kpmEventTrig;
    // DU_ALLOC(kpmEventTrig, sizeof(E2SM_KPM_EventTriggerDefinition_t));
    kpmEventTrig = (E2SM_KPM_EventTriggerDefinition_t *)calloc(1, sizeof(E2SM_KPM_EventTriggerDefinition_t));
    if(kpmEventTrig == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Event Trigger Definition failed, %d", __LINE__);
        return RFAILED;
    }
    DU_LOG("\nINFO   -->  E2SM-KPM : Decap Event Trigger Definition<<<<\n");

    rval = aper_decode(0, &asn_DEF_E2SM_KPM_EventTriggerDefinition, (void **)&kpmEventTrig, defini->buf, defini->size, 0, 0);
    if(rval.code == RC_FAIL || rval.code == RC_WMORE)
   {
      DU_LOG("\nERROR  -->  E2SM-KPM : Could not decode Event Trigger Definition, %d", __LINE__);
      return RFAILED;
   }
    xer_fprint(stdout, &asn_DEF_E2SM_KPM_EventTriggerDefinition, kpmEventTrig);
    DU_LOG("\nINFO  -->  E2SM-KPM : Report Period is %ld\n", kpmEventTrig->eventDefinition_formats.choice.eventDefinition_Format1->reportingPeriod);
    reportingPeriod = kpmEventTrig->eventDefinition_formats.choice.eventDefinition_Format1->reportingPeriod;
    ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_EventTriggerDefinition, kpmEventTrig);
    return ROK;
}

/*******************************************************************
 *
 * @brief Get the index of measurement name for cell metrics
 *
 * @details
 *
 *    Function : kpmCellMeasNameIsSupport
 *
 *    Functionality: Return the index of measurement name
 *
 * @param[in] measNameBuf, uint8_t*
 * @return index, int
 *
 ******************************************************************/

int kpmCellMeasNameIsSupport(uint8_t *measNameBuf){
    for(int i=0;i<SUPPORTED_CELL_MEAS_ITEM;i++){
        if(strcmp((const char*)measNameBuf, cellMetricName[i]) == 0)
            return i;
    }
    return -1; 
}

/*******************************************************************
 *
 * @brief Get the index of measurement name for slice metrics
 *
 * @details
 *
 *    Function : kpmSliceMeasNameIsSupport
 *
 *    Functionality: Return the index of measurement name
 *
 * @param[in] measNameBuf, uint8_t*
 * @return index, int
 *
 ******************************************************************/

int kpmSliceMeasNameIsSupport(uint8_t *measNameBuf){
    for(int i=0;i<SUPPORTED_SLICE_MEAS_ITEM;i++){
        if(strcmp((const char*)measNameBuf, sliceMetricName[i]) == 0)
            return i;
    }
    return -1;
}

/*******************************************************************
 *
 * @brief Process the action definition format 1
 *
 * @details
 *
 *    Function : kpmProcActionDefinitionFmt1
 *
 *    Functionality: Process the E2SM_KPM_ActionDefinition_Format1_t and get subscription information for cell metric
 *
 * @param[in] actionDefinFormat1, E2SM_KPM_ActionDefinition_Format1_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcActionDefinitionFmt1(E2SM_KPM_ActionDefinition_Format1_t *actionDefinFormat1){
    int index;
    sizeOfMeasNameFmt1 = 0;
    DU_LOG("\nINFO  -->  E2SM-KPM : Granularity Period is %ld\n", actionDefinFormat1->granulPeriod);
    for(int i=0;i<actionDefinFormat1->measInfoList.list.count;i++){
        if(actionDefinFormat1->measInfoList.list.array[i]->measType.present == MeasurementType_PR_measID){
            DU_LOG("\nERROR  -->  E2SM-KPM : Measurement ID is not supported \n");
            return RFAILED;
        }
        else if(actionDefinFormat1->measInfoList.list.array[i]->measType.present == MeasurementType_PR_measName){
            index = kpmCellMeasNameIsSupport(actionDefinFormat1->measInfoList.list.array[i]->measType.choice.measName.buf);
            if(index != -1){
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Cell metric is valid", actionDefinFormat1->measInfoList.list.array[i]->measType.choice.measName.buf);
                memcpy(indicationMeasNameFmt1[sizeOfMeasNameFmt1], cellMetricName[index], strlen((const char*)cellMetricName[index])+1);
                sizeOfMeasNameFmt1++;
            }
            else{
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Cell metric is invalid \n", actionDefinFormat1->measInfoList.list.array[i]->measType.choice.measName.buf);
                return RFAILED;
            }
        }
        else{
            DU_LOG("\nINFO  -->  E2SM-KPM : Measurement type is invalid \n");
            return RFAILED;
        }
    }
    for(int i=0;i<sizeOfMeasNameFmt1;i++){
        DU_LOG("\nINFO  -->  E2SM-KPM : # %d Measurement name is \"%s\"", i, indicationMeasNameFmt1[i]);
    }
    if(sizeOfMeasNameFmt1>0)
        return ROK;
    else
        return RFAILED;
}

/*******************************************************************
 *
 * @brief Process the action definition format 1 of KPMv3
 *
 * @details
 *
 *    Function : kpmProcActionDefinitionFmt1V3
 *
 *    Functionality: Process the E2SM_KPM_ActionDefinition_Format1_v300_t and get subscription information for cell metric
 *
 * @param[in] actionDefinFormat1, E2SM_KPM_ActionDefinition_Format1_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcActionDefinitionFmt1V3(E2SM_KPM_ActionDefinition_Format1_v300_t *actionDefinFormat1){
    int index;
    sizeOfMeasNameFmt1 = 0;
    DU_LOG("\nINFO  -->  E2SM-KPM : Granularity Period is %ld\n", actionDefinFormat1->granulPeriod);
    for(int i=0;i<actionDefinFormat1->measInfoList.list.count;i++){
        if(actionDefinFormat1->measInfoList.list.array[i]->measType.present == MeasurementType_PR_measID){
            DU_LOG("\nERROR  -->  E2SM-KPM : Measurement ID is not supported \n");
            return RFAILED;
        }
        else if(actionDefinFormat1->measInfoList.list.array[i]->measType.present == MeasurementType_PR_measName){
            index = kpmCellMeasNameIsSupport(actionDefinFormat1->measInfoList.list.array[i]->measType.choice.measName.buf);
            if(index != -1){
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Cell metric is valid", actionDefinFormat1->measInfoList.list.array[i]->measType.choice.measName.buf);
                memcpy(indicationMeasNameFmt1[sizeOfMeasNameFmt1], cellMetricName[index], strlen((const char*)cellMetricName[index])+1);
                sizeOfMeasNameFmt1++;
            }
            else{
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Cell metric is invalid \n", actionDefinFormat1->measInfoList.list.array[i]->measType.choice.measName.buf);
                return RFAILED;
            }
        }
        else{
            DU_LOG("\nINFO  -->  E2SM-KPM : Measurement type is invalid \n");
            return RFAILED;
        }
    }
    for(int i=0;i<sizeOfMeasNameFmt1;i++){
        DU_LOG("\nINFO  -->  E2SM-KPM : # %d Measurement name is \"%s\"", i, indicationMeasNameFmt1[i]);
    }
    if(sizeOfMeasNameFmt1>0)
        return ROK;
    else
        return RFAILED;
}

/*******************************************************************
 *
 * @brief Process the action definition format 3
 *
 * @details
 *
 *    Function : kpmProcActionDefinitionFmt3
 *
 *    Functionality: Process the E2SM_KPM_ActionDefinition_Format3_t and get subscription information for slice metric
 *
 * @param[in] actionDefinFormat3, E2SM_KPM_ActionDefinition_Format3_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcActionDefinitionFmt3(E2SM_KPM_ActionDefinition_Format3_t *actionDefinFormat3){
    int index;
    sizeOfMeasNameFmt3 = 0;
    DU_LOG("\nINFO  -->  E2SM-KPM : Granularity Period is %ld\n", actionDefinFormat3->granulPeriod);
    for(int i=0;i<actionDefinFormat3->measCondList.list.count;i++){
        if(actionDefinFormat3->measCondList.list.array[i]->measType.present == MeasurementType_PR_measID){
            DU_LOG("\nERROR  -->  E2SM-KPM : Measurement ID is not supported \n");
            return RFAILED;
        }
        else if(actionDefinFormat3->measCondList.list.array[i]->measType.present == MeasurementType_PR_measName){
            index = kpmSliceMeasNameIsSupport(actionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf);
            if(index != -1){
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Slice metric is valid", actionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf);
                memcpy(indicationMeasNameFmt2[sizeOfMeasNameFmt3], sliceMetricName[index], strlen((const char*)sliceMetricName[index])+1);
                sizeOfMeasNameFmt3++;
            }else{
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Slice metric is invalid \n", actionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf);
                return RFAILED;
            }
        }
        else{
            DU_LOG("\nINFO  -->  E2SM-KPM : Measurement type is invalid \n");
        }
    }
    for(int i=0;i<sizeOfMeasNameFmt3;i++){
        DU_LOG("\nINFO  -->  E2SM-KPM : # %d Measurement name is \"%s\"", i, indicationMeasNameFmt2[i]);
    }
    if(sizeOfMeasNameFmt3>0)
        return ROK;
    else
        return RFAILED;
}

/*******************************************************************
 *
 * @brief Process the action definition format 3 of KPMv3
 *
 * @details
 *
 *    Function : kpmProcActionDefinitionFmt3V3
 *
 *    Functionality: Process the E2SM_KPM_ActionDefinition_Format3_v300_t and get subscription information for slice metric
 *
 * @param[in] actionDefinFormat3, E2SM_KPM_ActionDefinition_Format3_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcActionDefinitionFmt3V3(E2SM_KPM_ActionDefinition_Format3_v300_t *actionDefinFormat3){
    int index;
    sizeOfMeasNameFmt3 = 0;
    DU_LOG("\nINFO  -->  E2SM-KPM : Granularity Period is %ld\n", actionDefinFormat3->granulPeriod);
    for(int i=0;i<actionDefinFormat3->measCondList.list.count;i++){
        if(actionDefinFormat3->measCondList.list.array[i]->measType.present == MeasurementType_PR_measID){
            DU_LOG("\nERROR  -->  E2SM-KPM : Measurement ID is not supported \n");
            return RFAILED;
        }
        else if(actionDefinFormat3->measCondList.list.array[i]->measType.present == MeasurementType_PR_measName){
            index = kpmSliceMeasNameIsSupport(actionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf);    //從ric傳來的ActionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf定義了"DRB.UEThpDl.SNSSAI" ,"RRU.PrbUsedDl.SNSSAI.若odu支援就回傳index!=-1
            if(index != -1){
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Slice metric is valid", actionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf);            //從ric傳來的ActionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf定義了"DRB.UEThpDl.SNSSAI" ,"RRU.PrbUsedDl.SNSSAI
                memcpy(indicationMeasNameFmt2[sizeOfMeasNameFmt3], sliceMetricName[index], strlen((const char*)sliceMetricName[index])+1);  //複製DRB.UEThpDl.SNSSAI" ,"RRU.PrbUsedDl.SNSSAI到indicationMeasNameFmt2
                sizeOfMeasNameFmt3++;
            }else{
                DU_LOG("\nINFO  -->  E2SM-KPM : Measurement name \"%s\" of Slice metric is invalid \n", actionDefinFormat3->measCondList.list.array[i]->measType.choice.measName.buf);
                return RFAILED;
            }
        }
        else{
            DU_LOG("\nINFO  -->  E2SM-KPM : Measurement type is invalid \n");
        }
    }
    for(int i=0;i<sizeOfMeasNameFmt3;i++){
        DU_LOG("\nINFO  -->  E2SM-KPM : # %d Measurement name is \"%s\"", i, indicationMeasNameFmt2[i]);
    }
    if(sizeOfMeasNameFmt3>0)
        return ROK;
    else
        return RFAILED;
}

/*******************************************************************
 *
 * @brief Process the action definition
 *
 * @details
 *
 *    Function : kpmProcActionDefinition
 *
 *    Functionality: Process the Action Defintion in RIC Subscription Request
 *
 * @param[in] ricdifin, RICactionDefinition_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcActionDefinition(RICactionDefinition_t *ricdifin){
    uint8_t ret = ROK;
    asn_dec_rval_t  rval;
    E2SM_KPM_ActionDefinition_t *actionDefin = (E2SM_KPM_ActionDefinition_t*)calloc(1, sizeof(E2SM_KPM_ActionDefinition_t));
    DU_LOG("\nINFO   -->  E2SM-KPM : Decap Action Definition<<<<\n");
    // DU_ALLOC(actionDefin, sizeof(E2SM_KPM_ActionDefinition_t));
    if(actionDefin == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Action Definition failed, %d", __LINE__);
        return RFAILED;
    }
    rval = aper_decode(0, &asn_DEF_E2SM_KPM_ActionDefinition, (void **)&actionDefin, ricdifin->buf, ricdifin->size, 0, 0);
    if(rval.code == RC_FAIL || rval.code == RC_WMORE)
    {
        DU_LOG("\nERROR  -->  E2SM-KPM : Could not decode Action Definition, %d", __LINE__);
        return RFAILED;
    }
    
    xer_fprint(stdout, &asn_DEF_E2SM_KPM_ActionDefinition, actionDefin);
    DU_LOG("\nINFO   -->  E2SM-KPM: RIC Style Type = %ld", actionDefin->ric_Style_Type);
    
    switch(actionDefin->actionDefinition_formats.present){
        case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format1:
            ret = kpmProcActionDefinitionFmt1(actionDefin->actionDefinition_formats.choice.actionDefinition_Format1);
            if(ret != ROK){
                DU_LOG("\nERROR  -->  E2SM-KPM: Invaild Action Definition of Format 1, %d", __LINE__);
                return RFAILED;
            }
            kpmEnableIndication(&kpmCellIndicationEnable);
            reportingPeriodFmt1 = reportingPeriod; 
            DU_LOG("\nINFO   -->  E2SM-KPM: Reporting Period format 1 = %d", reportingPeriodFmt1);
            break;
        case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format3:
            ret = kpmProcActionDefinitionFmt3(actionDefin->actionDefinition_formats.choice.actionDefinition_Format3);
            if(ret != ROK){
                DU_LOG("\nERROR  -->  E2SM-KPM: Invaild Action Definition of Format 3, %d", __LINE__);
                return RFAILED;
            }
            kpmEnableIndication(&kpmSliceIndicationEnable);
            reportingPeriodFmt2 = reportingPeriod; 
            DU_LOG("\nINFO   -->  E2SM-KPM: Reporting Period format 3 = %d", reportingPeriodFmt2);
        break;
        default:
            DU_LOG("\nINFO   -->  E2SM-KPM: Invaild Action Definition format %d, %d", actionDefin->actionDefinition_formats.present, __LINE__);
            return RFAILED;
    }
    kpmIndicationV3Enable = 0;
    ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_ActionDefinition, actionDefin);
    return ROK;
}

/*******************************************************************
 *
 * @brief Process the action definition of KPMv3
 *
 * @details
 *
 *    Function : kpmProcActionDefinitionV3
 *
 *    Functionality: Process the Action Defintion in RIC Subscription Request
 *
 * @param[in] ricdifin, RICactionDefinition_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmProcActionDefinitionV3(RICactionDefinition_t *ricdifin){
    uint8_t ret = ROK;
    asn_dec_rval_t  rval;
    E2SM_KPM_ActionDefinition_v300_t *actionDefin = (E2SM_KPM_ActionDefinition_v300_t*)calloc(1, sizeof(E2SM_KPM_ActionDefinition_v300_t));
    DU_LOG("\nINFO   -->  E2SM-KPMv3 : Decap Action Definition v3<<<<\n");
    // DU_ALLOC(actionDefin, sizeof(E2SM_KPM_ActionDefinition_v300_t));
    if(actionDefin == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Action Definition failed, %d", __LINE__);
        return RFAILED;
    }

    rval = aper_decode(0, &asn_DEF_E2SM_KPM_ActionDefinition_v300, (void **)&actionDefin, ricdifin->buf, ricdifin->size, 0, 0);    //解碼ric傳來的action defi.
    if(rval.code == RC_FAIL || rval.code == RC_WMORE)
    {
        DU_LOG("\nERROR  -->  E2SM-KPM : Could not decode Action Definition, %d", __LINE__);
        return RFAILED;
    }
    
    xer_fprint(stdout, &asn_DEF_E2SM_KPM_ActionDefinition_v300, actionDefin);
    DU_LOG("\nINFO   -->  E2SM-KPMv3: RIC Style Type = %ld", actionDefin->ric_Style_Type);

    switch(actionDefin->actionDefinition_formats.present){
        case E2SM_KPM_ActionDefinition_v300__actionDefinition_formats_PR_actionDefinition_Format1:
            ret = kpmProcActionDefinitionFmt1V3(actionDefin->actionDefinition_formats.choice.actionDefinition_Format1);
            if(ret != ROK){
                DU_LOG("\nERROR  -->  E2SM-KPM: Invaild Action Definition of Format 1, %d", __LINE__);
                return RFAILED;
            }
            kpmEnableIndication(&kpmCellIndicationEnable);
            reportingPeriodFmt1 = reportingPeriod; 
            DU_LOG("\nINFO   -->  E2SM-KPM: Reporting Period format 1 = %d", reportingPeriodFmt1);
            break;
        case E2SM_KPM_ActionDefinition_v300__actionDefinition_formats_PR_actionDefinition_Format3:
            ret = kpmProcActionDefinitionFmt3V3(actionDefin->actionDefinition_formats.choice.actionDefinition_Format3);   //處理ric傳來的action defi.
            if(ret != ROK){
                DU_LOG("\nERROR  -->  E2SM-KPM: Invaild Action Definition of Format 3, %d", __LINE__);
                return RFAILED;
            }
            kpmEnableIndication(&kpmSliceIndicationEnable);
            reportingPeriodFmt2 = reportingPeriod; 
            DU_LOG("\nINFO   -->  E2SM-KPM: Reporting Period format 3 = %d", reportingPeriodFmt2);
        break;
        default:
            DU_LOG("\nINFO   -->  E2SM-KPM: Invaild Action Definition format %d, %d", actionDefin->actionDefinition_formats.present, __LINE__);
            return RFAILED;
    }
    kpmIndicationV3Enable = 1;
    ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_ActionDefinition, actionDefin);
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_IndicationHeader_Format1_t 
 *
 * @details
 *
 *    Function : kpmFreeIndicationHeader
 *
 *    Functionality: freeing the memory allocated for E2SM_KPM_IndicationHeader_Format1_t
 *
 * @param[in] indicaHeaderFormat1, E2SM_KPM_IndicationHeader_Format1_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeIndicationHeader(E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1){
    if(indicaHeaderFormat1->colletStartTime.size>0){
        if(indicaHeaderFormat1->colletStartTime.buf){
            DU_FREE(indicaHeaderFormat1->colletStartTime.buf, indicaHeaderFormat1->colletStartTime.size);
        }
    }
    if(indicaHeaderFormat1->senderName){
        kpmFreeOctetString(indicaHeaderFormat1->senderName); 
    }
    if(indicaHeaderFormat1->senderType){
        kpmFreeOctetString(indicaHeaderFormat1->senderType); 
    }
    if(indicaHeaderFormat1->vendorName){
        kpmFreeOctetString(indicaHeaderFormat1->vendorName); 
    }
    if(indicaHeaderFormat1->fileFormatversion){
        kpmFreeOctetString(indicaHeaderFormat1->fileFormatversion); 
    }
   return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_IndicationHeader_Format1_t of KPMv3
 *
 * @details
 *
 *    Function : kpmFreeIndicationHeaderV3
 *
 *    Functionality: freeing the memory allocated for E2SM_KPM_IndicationHeader_Format1_v300_t
 *
 * @param[in] indicaHeaderFormat1, E2SM_KPM_IndicationHeader_Format1_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeIndicationHeaderV3(E2SM_KPM_IndicationHeader_Format1_v300_t *indicaHeaderFormat1){
    if(indicaHeaderFormat1->colletStartTime.size>0){
        if(indicaHeaderFormat1->colletStartTime.buf){
            DU_FREE(indicaHeaderFormat1->colletStartTime.buf, indicaHeaderFormat1->colletStartTime.size);
        }
    }
    if(indicaHeaderFormat1->senderName){
        kpmFreeOctetString(indicaHeaderFormat1->senderName); 
    }
    if(indicaHeaderFormat1->senderType){
        kpmFreeOctetString(indicaHeaderFormat1->senderType); 
    }
    if(indicaHeaderFormat1->vendorName){
        kpmFreeOctetString(indicaHeaderFormat1->vendorName); 
    }
    if(indicaHeaderFormat1->fileFormatversion){
        kpmFreeOctetString(indicaHeaderFormat1->fileFormatversion); 
    }
   return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the optinal item of Indication Header format 1
 *
 * @details
 *
 *    Function : kpmFillIndicationHeaderFormat1Optional
 *
 *    Functionality: Fills the optional items of Indication Header format 1
 *
 * @param[in] indicaHeaderFormat1, E2SM_KPM_IndicationHeader_Format1_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationHeaderFormat1Optional(E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1){
    uint8_t ret = ROK;
    uint8_t sender[] = "NTUST";
    uint8_t type[] = "DU";
    uint8_t vendor[] = "OSC";

    DU_ALLOC(indicaHeaderFormat1->senderName, sizeof(PrintableString_t));
    if(indicaHeaderFormat1->senderName == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Sender Name failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(indicaHeaderFormat1->senderType, sizeof(PrintableString_t));
    if(indicaHeaderFormat1->senderType == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Sender Type failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(indicaHeaderFormat1->vendorName, sizeof(PrintableString_t));
    if(indicaHeaderFormat1->vendorName == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Vendor Name failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillPrintableString(indicaHeaderFormat1->senderName, sender);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Sender Name, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillPrintableString(indicaHeaderFormat1->senderType, type);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Sender Type, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillPrintableString(indicaHeaderFormat1->vendorName, vendor);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Vendor Name, %d", __LINE__);
        return RFAILED;
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the optinal item of Indication Header format 1 of KPMv3
 *
 * @details
 *
 *    Function : kpmFillIndicationHeaderFormat1OptionalV3
 *
 *    Functionality: Fills the optional items of Indication Header format 1
 *
 * @param[in] indicaHeaderFormat1, E2SM_KPM_IndicationHeader_Format1_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationHeaderFormat1OptionalV3(E2SM_KPM_IndicationHeader_Format1_v300_t *indicaHeaderFormat1){
    uint8_t ret = ROK;
    uint8_t sender[] = "NTUST";
    uint8_t type[] = "DU";
    uint8_t vendor[] = "OSC";

    DU_ALLOC(indicaHeaderFormat1->senderName, sizeof(PrintableString_t));
    if(indicaHeaderFormat1->senderName == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Sender Name failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(indicaHeaderFormat1->senderType, sizeof(PrintableString_t));
    if(indicaHeaderFormat1->senderType == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Sender Type failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(indicaHeaderFormat1->vendorName, sizeof(PrintableString_t));
    if(indicaHeaderFormat1->vendorName == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Vendor Name failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillPrintableString(indicaHeaderFormat1->senderName, sender);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Sender Name, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillPrintableString(indicaHeaderFormat1->senderType, type);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Sender Type, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillPrintableString(indicaHeaderFormat1->vendorName, vendor);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Vendor Name, %d", __LINE__);
        return RFAILED;
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Header format 1
 *
 * @details
 *
 *    Function : kpmFillIndicationHeaderFormat1
 *
 *    Functionality: Fills Indication Header format 1 of RIC Indication
 *
 * @param[in] indicaHeaderFormat1, E2SM_KPM_IndicationHeader_Format1_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationHeaderFormat1(E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1){
    uint8_t ret = ROK;    
    time_t now = time(NULL);
    unsigned long timestamp = (unsigned long)now;
    uint8_t ts[4];
    for(int i=0;i<4;i++){
        ts[i] = timestamp % 256;
        timestamp = timestamp / 256;
    }
    DU_ALLOC(indicaHeaderFormat1->colletStartTime.buf, 4 * sizeof(uint8_t));
    if(indicaHeaderFormat1->colletStartTime.buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Collect Start Time failed, %d", __LINE__);
        return RFAILED;
    }
    indicaHeaderFormat1->colletStartTime.size = 4;
    memcpy(indicaHeaderFormat1->colletStartTime.buf,ts, 4);
    ret = kpmFillIndicationHeaderFormat1Optional(indicaHeaderFormat1);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Indication Header of optional item, %d", __LINE__);
        return RFAILED;
    }
    return 0;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Header format 1 of KPMv3
 *
 * @details
 *
 *    Function : kpmFillIndicationHeaderFormat1V3
 *
 *    Functionality: Fills Indication Header format 1 of RIC Indication
 *
 * @param[in] indicaHeaderFormat1, E2SM_KPM_IndicationHeader_Format1_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationHeaderFormat1V3(E2SM_KPM_IndicationHeader_Format1_v300_t *indicaHeaderFormat1){
    uint8_t ret = ROK;    
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    uint64_t timestamp = (uint64_t)current_time.tv_sec * 1000000 + current_time.tv_usec;
    uint8_t ts[8];
    for(int i=0;i<8;i++){
        ts[i] = timestamp % 256;
        timestamp = timestamp / 256;
    }
    DU_ALLOC(indicaHeaderFormat1->colletStartTime.buf, 8 * sizeof(uint8_t));
    if(indicaHeaderFormat1->colletStartTime.buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Collect Start Time failed, %d", __LINE__);
        return RFAILED;
    }
    indicaHeaderFormat1->colletStartTime.size = 8;
    memcpy(indicaHeaderFormat1->colletStartTime.buf,ts, 8);
    ret = kpmFillIndicationHeaderFormat1OptionalV3(indicaHeaderFormat1);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Indication Header of optional item, %d", __LINE__);
        return RFAILED;
    }
    return 0;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Header
 *
 * @details
 *
 *    Function : kpmFillRicIndicationHeader
 *
 *    Functionality: Fills Indication Header of RIC Indication by E2AP Handler
 *
 * @param[in] indicaHeaderFormat1, RICindicationHeader_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRicIndicationHeader(RICindicationHeader_t *ricIndicationHeader){
    uint8_t ret = ROK;
    DU_LOG("\nINFO   -->  E2SM-KPM : Fill KPM RIC Indication Header");
    E2SM_KPM_IndicationHeader_t *indicaHeader;
    DU_ALLOC(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_t));
    if(indicaHeader == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Header failed, %d", __LINE__);
        return RFAILED;
    }
    E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1;
    DU_ALLOC(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_t));
    if(indicaHeaderFormat1 == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Header Format1 failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillIndicationHeaderFormat1(indicaHeaderFormat1);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Indication Header Format1, %d", __LINE__);
        return RFAILED;
    }
    indicaHeader->indicationHeader_formats.present = E2SM_KPM_IndicationHeader__indicationHeader_formats_PR_indicationHeader_Format1;
    indicaHeader->indicationHeader_formats.choice.indicationHeader_Format1 = indicaHeaderFormat1;
    xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationHeader, indicaHeader);

    uint8_t e2smbuffer[8192] = {0, };
    size_t e2smbuffer_size = 8192;
    asn_codec_ctx_t *opt_cod = 0;
    
    asn_enc_rval_t er =
    asn_encode_to_buffer(opt_cod,
          ATS_ALIGNED_BASIC_PER,
          &asn_DEF_E2SM_KPM_IndicationHeader,
          indicaHeader, e2smbuffer, e2smbuffer_size);

    if(er.encoded == -1){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not encode E2SM-KPM Indication Header, %d", __LINE__);
        kpmFreeIndicationHeader(indicaHeaderFormat1);
        DU_FREE(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_t));
        DU_FREE(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_t));
        return RFAILED;
    }
    else{
        DU_ALLOC(ricIndicationHeader->buf, er.encoded * sizeof(uint8_t));
        if(ricIndicationHeader->buf == NULL){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Header failed, %d", __LINE__);
            kpmFreeIndicationHeader(indicaHeaderFormat1);
            DU_FREE(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_t));
            DU_FREE(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_t));
            return RFAILED;
        }
        ricIndicationHeader->size = er.encoded;
        memcpy(ricIndicationHeader->buf,e2smbuffer,er.encoded);
    }

    kpmFreeIndicationHeader(indicaHeaderFormat1);
    DU_FREE(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_t));
    DU_FREE(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_t));
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Header of KPMv3
 *
 * @details
 *
 *    Function : kpmFillRicIndicationHeaderV3
 *
 *    Functionality: Fills Indication Header of RIC Indication by E2AP Handler
 *
 * @param[in] indicaHeaderFormat1, RICindicationHeader_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRicIndicationHeaderV3(RICindicationHeader_t *ricIndicationHeader){
    uint8_t ret = ROK;
    DU_LOG("\nINFO   -->  E2SM-KPM : Fill KPM RIC Indication Headerv3");
    E2SM_KPM_IndicationHeader_v300_t *indicaHeader;
    DU_ALLOC(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_v300_t));
    if(indicaHeader == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Header failed, %d", __LINE__);
        return RFAILED;
    }
    E2SM_KPM_IndicationHeader_Format1_v300_t *indicaHeaderFormat1;
    DU_ALLOC(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_v300_t));
    if(indicaHeaderFormat1 == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Header Format1 failed, %d", __LINE__);
        return RFAILED;
    }
    ret = kpmFillIndicationHeaderFormat1V3(indicaHeaderFormat1);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Failed to fill Indication Header Format1, %d", __LINE__);
        return RFAILED;
    }
    indicaHeader->indicationHeader_formats.present = E2SM_KPM_IndicationHeader_v300__indicationHeader_formats_PR_indicationHeader_Format1;
    indicaHeader->indicationHeader_formats.choice.indicationHeader_Format1 = indicaHeaderFormat1;
    xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationHeader_v300, indicaHeader);

    uint8_t e2smbuffer[8192] = {0, };
    size_t e2smbuffer_size = 8192;
    asn_codec_ctx_t *opt_cod = 0;
    asn_enc_rval_t er =
    asn_encode_to_buffer(opt_cod,
          ATS_ALIGNED_BASIC_PER,
          &asn_DEF_E2SM_KPM_IndicationHeader_v300,
          indicaHeader, e2smbuffer, e2smbuffer_size);

    if(er.encoded == -1){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not encode E2SM-KPM Indication Header, %d", __LINE__);
        kpmFreeIndicationHeaderV3(indicaHeaderFormat1);
        DU_FREE(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_v300_t));
        DU_FREE(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_v300_t));
        return RFAILED;
    }
    else{
        DU_ALLOC(ricIndicationHeader->buf, er.encoded * sizeof(uint8_t));
        if(ricIndicationHeader->buf == NULL){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Header failed, %d", __LINE__);
            kpmFreeIndicationHeaderV3(indicaHeaderFormat1);
            DU_FREE(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_v300_t));
            DU_FREE(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_v300_t));
            return RFAILED;
        }
        ricIndicationHeader->size = er.encoded;
        memcpy(ricIndicationHeader->buf,e2smbuffer,er.encoded);
    }

    kpmFreeIndicationHeaderV3(indicaHeaderFormat1);
    DU_FREE(indicaHeaderFormat1, sizeof(E2SM_KPM_IndicationHeader_Format1_v300_t));
    DU_FREE(indicaHeader, sizeof(E2SM_KPM_IndicationHeader_v300_t));
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the measurement record item for slice metrics
 *
 * @details
 *
 *    Function : kpmFillSliceMeasDataItem
 *
 *    Functionality: Fills MeasurementDataItem_t of Indication Message
 *
 * @param[in] measDataItem, MeasurementDataItem_t*, the destination to fill in
 * @param[in] intValue, unsigned long, the source
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillSliceMeasDataItem(MeasurementDataItem_t *measDataItem, unsigned long intValue){
    MeasurementRecordItem_t *measRecordItem;
    DU_ALLOC(measRecordItem, sizeof(MeasurementRecordItem_t));
    if(measRecordItem == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Record Item failed, %d", __LINE__);
        return RFAILED;
    }
    measRecordItem->present = MeasurementRecordItem_PR_integer;
    measRecordItem->choice.integer = intValue; 
    ASN_SEQUENCE_ADD(&measDataItem->measRecord.list, measRecordItem);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the measurement record item for cell metrics
 *
 * @details
 *
 *    Function : kpmFillCellMeasDataItem
 *
 *    Functionality: Fills MeasurementDataItem_t of Indication Message
 *
 * @param[in] measDataItem, MeasurementDataItem_t*, the destination to fill in
 * @param[in] present, MeasurementRecordItem_PR, the source type
 * @param[in] integer, int, the source if type is integer
 * @param[in] real, double, the source if type is real
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillCellMeasDataItem(MeasurementDataItem_t *item, MeasurementRecordItem_PR present, int integer, double real){
    MeasurementRecordItem_t *measRecordItem;
    DU_ALLOC(measRecordItem, sizeof(MeasurementRecordItem_t));
    if(measRecordItem == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Record Item failed, %d", __LINE__);
        return RFAILED;
    }
    
    if(present == MeasurementRecordItem_PR_integer){
        measRecordItem->present = MeasurementRecordItem_PR_integer;
        measRecordItem->choice.integer = integer;
    }
    else if(present == MeasurementRecordItem_PR_real){
        measRecordItem->present = MeasurementRecordItem_PR_real;
        measRecordItem->choice.real = real;
    }
    else if(present == MeasurementRecordItem_PR_noValue){
        measRecordItem->present = MeasurementRecordItem_PR_noValue;
    }
    else{
        DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Measurement Record Item, %d", __LINE__);
        return RFAILED;
    }
    ASN_SEQUENCE_ADD(&item->measRecord.list, measRecordItem);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Message format 1 for cell metrics
 *
 * @details
 *
 *    Function : kpmFillIndicationMessageFormat1
 *
 *    Functionality: Fills E2SM_KPM_IndicationMessage_Format1_t of Indication Message
 *
 * @param[in] indicaMessageFormat1, E2SM_KPM_IndicationMessage_Format1_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationMessageFormat1(E2SM_KPM_IndicationMessage_Format1_t *indicaMessageFormat1){
    uint8_t ret = ROK;
    int index;

    MeasurementDataItem_t *measDataItem;
    DU_ALLOC(measDataItem, sizeOfMeasNameFmt1 * sizeof(MeasurementDataItem_t));
    if(measDataItem == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Data Item failed, %d", __LINE__);
        return RFAILED;
    }
    MeasurementInfoList_t *measInfoList;
    DU_ALLOC(measInfoList, sizeof(MeasurementInfoList_t));
    if(measInfoList == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Info List failed, %d", __LINE__);
        return RFAILED;
    }
    MeasurementInfoItem_t *measInfoItem;
    DU_ALLOC(measInfoItem, sizeOfMeasNameFmt1 * sizeof(MeasurementInfoItem_t));
    if(measInfoItem == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Info Item failed, %d", __LINE__);
        return RFAILED;
    }

    for(int i=0;i<sizeOfMeasNameFmt1;i++){
        index = kpmCellMeasNameIsSupport(indicationMeasNameFmt1[i]);
        if(index == 0){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_real, 0, (double)kpmCellPmDb.avgThpDl);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %f", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgThpDl);
        }
        else if(index == 1){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_integer, kpmCellPmDb.avgUsedPrb, 0);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %d", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgUsedPrb);
        }
        else if(index == 2){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_integer, kpmCellPmDb.avgTotalPrb, 0);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %d", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgTotalPrb);
        }
        else if(index == 3){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_integer, (int)kpmCellPmDb.avgUsagePrb, 0);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %d", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgUsagePrb);
        }else{
            DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Cell Measurement name, %d", __LINE__);
            return RFAILED;
        }
    }

    indicaMessageFormat1->measInfoList = measInfoList;
    for(int i=0;i<sizeOfMeasNameFmt1;i++){
        ret = kpmFillMeasInfoItem(measInfoItem+i, (uint8_t*)indicationMeasNameFmt1[i], strlen((const char*)indicationMeasNameFmt1[i]));
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Info Item, %d", __LINE__);
            return RFAILED;
        }
        ASN_SEQUENCE_ADD(&indicaMessageFormat1->measInfoList->list, measInfoItem+i);
        ASN_SEQUENCE_ADD(&indicaMessageFormat1->measData.list, measDataItem+i);
    }
    xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, indicaMessageFormat1);
    return ROK; 
}

/*******************************************************************
 *
 * @brief Fill in the Indication Message format 1 for cell metrics of KPMv3
 *
 * @details
 *
 *    Function : kpmFillIndicationMessageFormat1V3
 *
 *    Functionality: Fills E2SM_KPM_IndicationMessage_Format1_v300_t of Indication Message
 *
 * @param[in] indicaMessageFormat1, E2SM_KPM_IndicationMessage_Format1_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationMessageFormat1V3(E2SM_KPM_IndicationMessage_Format1_v300_t *indicaMessageFormat1){
    uint8_t ret = ROK;
    int index;

    MeasurementDataItem_t *measDataItem;
    DU_ALLOC(measDataItem, sizeOfMeasNameFmt1 * sizeof(MeasurementDataItem_t));
    if(measDataItem == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Data Item failed, %d", __LINE__);
        return RFAILED;
    }
    MeasurementInfoList_v300_t *measInfoList;
    DU_ALLOC(measInfoList, sizeof(MeasurementInfoList_v300_t));
    if(measInfoList == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Info List failed, %d", __LINE__);
        return RFAILED;
    }
    MeasurementInfoItem_v300_t *measInfoItem;
    DU_ALLOC(measInfoItem, sizeOfMeasNameFmt1 * sizeof(MeasurementInfoItem_v300_t));
    if(measInfoItem == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Info Item failed, %d", __LINE__);
        return RFAILED;
    }

    for(int i=0;i<sizeOfMeasNameFmt1;i++){
        index = kpmCellMeasNameIsSupport(indicationMeasNameFmt1[i]);
        if(index == 0){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_real, 0, (double)kpmCellPmDb.avgThpDl);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %f", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgThpDl);
        }
        else if(index == 1){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_integer, kpmCellPmDb.avgUsedPrb, 0);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %d", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgUsedPrb);
        }
        else if(index == 2){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_integer, kpmCellPmDb.avgTotalPrb, 0);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %d", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgTotalPrb);
        }
        else if(index == 3){
            ret = kpmFillCellMeasDataItem(measDataItem + i, MeasurementRecordItem_PR_integer, (int)kpmCellPmDb.avgUsagePrb, 0);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                return RFAILED;
            }
            DU_LOG("\nINFO   -->  E2SM-KPM : # %d Cell Measurement \"%s\" : %d", i, indicationMeasNameFmt1[index], kpmCellPmDb.avgUsagePrb);
        }else{
            DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Cell Measurement name, %d", __LINE__);
            return RFAILED;
        }
    }

    indicaMessageFormat1->measInfoList = measInfoList;
    for(int i=0;i<sizeOfMeasNameFmt1;i++){
        ret = kpmFillMeasInfoItemV3(measInfoItem+i, (uint8_t*)indicationMeasNameFmt1[i], strlen((const char*)indicationMeasNameFmt1[i]));
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Info Item, %d", __LINE__);
            return RFAILED;
        }
        ASN_SEQUENCE_ADD(&indicaMessageFormat1->measInfoList->list, measInfoItem+i);
        ASN_SEQUENCE_ADD(&indicaMessageFormat1->measData.list, measDataItem+i);
    }
    xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1_v300, indicaMessageFormat1);
    return 0; 
}

/*******************************************************************
 *
 * @brief Fill in the matching conditional item for slice information
 *
 * @details
 *
 *    Function : kpmFillSliceMeasCondItem
 *
 *    Functionality: Fills kpmFillSliceMeasCondItem of Indication Message
 *
 * @param[in] measCondItem, MeasurementCondUEidItem_t*, the destination
 * @param[in] measureName, char[], the measure name source
 * @param[in] snssai, Snssai*, the slice ID source
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillSliceMeasCondItem(MeasurementCondUEidItem_t *measCondItem, char measureName[], Snssai snssai){
    uint8_t ret = ROK;
    MatchingCondItem_t *matchCondItem;
    DU_ALLOC(matchCondItem, sizeof(MatchingCondItem_t));
    if(matchCondItem == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Matching Conditional Item failed");
        return RFAILED;
    }

    measCondItem->measType.present = MeasurementType_PR_measName;
    ret = kpmFillOctetString(&measCondItem->measType.choice.measName, measureName);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Name, %d", __LINE__);
        return RFAILED;
    }
    matchCondItem->present = MatchingCondItem_PR_measLabel;
    DU_ALLOC(matchCondItem->choice.measLabel, sizeof(MeasurementLabel_t));
    if(matchCondItem->choice.measLabel == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(matchCondItem->choice.measLabel->sliceID, sizeof(S_NSSAI_t));
    if(matchCondItem->choice.measLabel->sliceID == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(matchCondItem->choice.measLabel->sliceID->sD, sizeof(SD_t));
    if(matchCondItem->choice.measLabel->sliceID->sD == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID - SD of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    matchCondItem->choice.measLabel->sliceID->sST.size = 1;
    DU_ALLOC(matchCondItem->choice.measLabel->sliceID->sST.buf, sizeof(uint8_t));
    if(matchCondItem->choice.measLabel->sliceID->sST.buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID - SST of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    memcpy(matchCondItem->choice.measLabel->sliceID->sST.buf, &snssai.sst, 1);
    matchCondItem->choice.measLabel->sliceID->sD->size = 3;
    DU_ALLOC(matchCondItem->choice.measLabel->sliceID->sD->buf, sizeof(uint8_t));
    if(matchCondItem->choice.measLabel->sliceID->sD->buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID - SD Octet String of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    memcpy(matchCondItem->choice.measLabel->sliceID->sD->buf, snssai.sd, 3);
    ASN_SEQUENCE_ADD(&measCondItem->matchingCond.list, matchCondItem);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the matching conditional item for slice information of KPMv3
 *
 * @details
 *
 *    Function : kpmFillSliceMeasCondItemV3
 *
 *    Functionality: Fills MeasurementCondUEidItem_v300_t of Indication Message
 *
 * @param[in] measCondItem, MeasurementCondUEidItem_v300_t*, the destination
 * @param[in] measureName, char[], the measure name source
 * @param[in] snssai, Snssai*, the slice ID source
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillSliceMeasCondItemV3(MeasurementCondUEidItem_v300_t *measCondItem, char measureName[], Snssai snssai){
    uint8_t ret = ROK;
    MatchingCondItem_v300_t *matchCondItem;
    DU_ALLOC(matchCondItem, sizeof(MatchingCondItem_v300_t));
    if(matchCondItem == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Matching Conditional Item failed");
        return RFAILED;
    }

    measCondItem->measType.present = MeasurementType_PR_measName;
    ret = kpmFillOctetString(&measCondItem->measType.choice.measName, measureName);
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Name, %d", __LINE__);
        return RFAILED;
    }

    matchCondItem->matchingCondChoice.present = MatchingCondItem_Choice_PR_measLabel;
    DU_ALLOC(matchCondItem->matchingCondChoice.choice.measLabel, sizeof(MeasurementLabel_t));
    if(matchCondItem->matchingCondChoice.choice.measLabel == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(matchCondItem->matchingCondChoice.choice.measLabel->sliceID, sizeof(S_NSSAI_t));
    if(matchCondItem->matchingCondChoice.choice.measLabel->sliceID == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    DU_ALLOC(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sD, sizeof(SD_t));
    if(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sD == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID - SD of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sST.size = 1;
    DU_ALLOC(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sST.buf, sizeof(uint8_t));
    if(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sST.buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID - SST of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    memcpy(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sST.buf, &snssai.sst, 1);
    matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sD->size = 3;
    DU_ALLOC(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sD->buf, sizeof(uint8_t));
    if(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sD->buf == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Label Slice ID - SD Octet String of Matching Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }
    memcpy(matchCondItem->matchingCondChoice.choice.measLabel->sliceID->sD->buf, snssai.sd, 3);
    ASN_SEQUENCE_ADD(&measCondItem->matchingCond.list, matchCondItem);
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Message format 2 for slice metrics
 *
 * @details
 *
 *    Function : kpmFillIndicationMessageFormat2
 *
 *    Functionality: Fills E2SM_KPM_IndicationMessage_Format2_t of Indication Message
 *
 * @param[in] indicaMessageFormat2, E2SM_KPM_IndicationMessage_Format2_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationMessageFormat2(E2SM_KPM_IndicationMessage_Format2_t *indicaMessageFormat2){
    uint8_t ret = ROK;
    int styleSize = SUPPORTED_SLICE_MEAS_ITEM;
    int measSize = MAX_SIZE_OF_SLICE;
    int cnt = 0;
    int index;
    MeasurementDataItem_t *measDataItem;
    DU_ALLOC(measDataItem, measSize * styleSize * sizeof(MeasurementDataItem_t));
    if(measDataItem == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement data item failed, %d", __LINE__);
        return RFAILED;
    }
    MeasurementCondUEidItem_t *measCondItem;
    DU_ALLOC(measCondItem, measSize * styleSize * sizeof(MeasurementCondUEidItem_t));
    if(measCondItem == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }

    for(int i=0;i<kpmSlicePmDb.numOfSlice;i++){
        for(int j=0;j<sizeOfMeasNameFmt3;j++){
            index = kpmSliceMeasNameIsSupport(indicationMeasNameFmt2[j]);
            if(index == 0){
                ret = kpmFillSliceMeasDataItem(measDataItem + cnt, kpmSlicePmDb.avgThpDl[i]);
                if(ret != ROK){
                    DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                    return RFAILED;
                }
                DU_LOG("\nINFO   -->  E2SM-KPM : # %d Slice Measurement \"%s\" in slice {%d-%d%d%d} : %d", j, indicationMeasNameFmt2[index],
                kpmSlicePmDb.snssai[i].sst, kpmSlicePmDb.snssai[i].sd[0], kpmSlicePmDb.snssai[i].sd[1],
                kpmSlicePmDb.snssai[i].sd[2], kpmSlicePmDb.avgThpDl[i]);
                DU_LOG("\nINFO   -->  kpmFillIndicationMessageFormat2kpmFillIndicationMessageFormat2kpmFillIndicationMessageFormat2");

            }
            else if(index == 1){
                ret = kpmFillSliceMeasDataItem(measDataItem + cnt, kpmSlicePmDb.avgUsedPrb[i]);
                if(ret != ROK){
                    DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                    return RFAILED;
                }
                DU_LOG("\nINFO   -->  E2SM-KPM : # %d Slice Measurement \"%s\" in slice {%d-%d%d%d} : %d", j, indicationMeasNameFmt2[index],
                kpmSlicePmDb.snssai[i].sst, kpmSlicePmDb.snssai[i].sd[0], kpmSlicePmDb.snssai[i].sd[1],
                kpmSlicePmDb.snssai[i].sd[2], kpmSlicePmDb.avgUsedPrb[i]);
            }
            else if(index = -1){
                DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Cell Measurement name, %d", __LINE__);
                return RFAILED;
            }

            ret = kpmFillSliceMeasCondItem(measCondItem+cnt, indicationMeasNameFmt2[index], kpmSlicePmDb.snssai[i]);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Conditional item for slice, %d", __LINE__);
                return RFAILED;
            }
            ASN_SEQUENCE_ADD(&indicaMessageFormat2->measData.list, measDataItem+cnt);
            ASN_SEQUENCE_ADD(&indicaMessageFormat2->measCondUEidList.list, measCondItem+cnt);
            cnt = cnt + 1;
        }
    }

    xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format2, indicaMessageFormat2);
    return ROK; 
}

/*******************************************************************
 *
 * @brief Fill in the Indication Message format 2 for slice metrics of KPMv3
 *
 * @details
 *
 *    Function : kpmFillIndicationMessageFormat2V3
 *
 *    Functionality: Fills E2SM_KPM_IndicationMessage_Format2_v300_t of Indication Message
 *
 * @param[in] indicaMessageFormat2, E2SM_KPM_IndicationMessage_Format2_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillIndicationMessageFormat2V3(E2SM_KPM_IndicationMessage_Format2_v300_t *indicaMessageFormat2){
    uint8_t ret = ROK;
    int styleSize = SUPPORTED_SLICE_MEAS_ITEM;
    int measSize = MAX_SIZE_OF_SLICE;
    int cnt = 0;
    int index = 0;
    MeasurementDataItem_t *measDataItem;
    DU_ALLOC(measDataItem, measSize * styleSize * sizeof(MeasurementDataItem_t));
    if(measDataItem == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement data item failed, %d", __LINE__);
        return RFAILED;
    }
    MeasurementCondUEidItem_t *measCondItem;
    DU_ALLOC(measCondItem, measSize * styleSize * sizeof(MeasurementCondUEidItem_t));
    if(measCondItem == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Measurement Conditional Item failed, %d", __LINE__);
        return RFAILED;
    }

    for(int i=0;i<kpmSlicePmDb.numOfSlice;i++){
        for(int j=0;j<sizeOfMeasNameFmt3;j++){
            index = kpmSliceMeasNameIsSupport(indicationMeasNameFmt2[j]);
            if(index == 0){
                ret = kpmFillSliceMeasDataItem(measDataItem + cnt, kpmSlicePmDb.avgThpDl[i]);
                if(ret != ROK){
                    DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                    return RFAILED;
                }
                DU_LOG("\nINFO   -->  E2SM-KPM : # %d Slice Measurement \"%s\" in slice {%d-%d%d%d} : %d", j, indicationMeasNameFmt2[index],
                kpmSlicePmDb.snssai[i].sst, kpmSlicePmDb.snssai[i].sd[0], kpmSlicePmDb.snssai[i].sd[1],
                kpmSlicePmDb.snssai[i].sd[2], kpmSlicePmDb.avgThpDl[i]);
            }
            else if(index == 1){
                ret = kpmFillSliceMeasDataItem(measDataItem + cnt, kpmSlicePmDb.avgUsedPrb[i]);
                if(ret != ROK){
                    DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Data Item, %d", __LINE__);
                    return RFAILED;
                }
                DU_LOG("\nINFO   -->  E2SM-KPM : # %d Slice Measurement \"%s\" in slice {%d-%d%d%d} : %d", j, indicationMeasNameFmt2[index],
                kpmSlicePmDb.snssai[i].sst, kpmSlicePmDb.snssai[i].sd[0], kpmSlicePmDb.snssai[i].sd[1],
                kpmSlicePmDb.snssai[i].sd[2], kpmSlicePmDb.avgUsedPrb[i]);
                DU_LOG("\nINFO   -->  kpmFillIndicationMessageFormat2V3kpmFillIndicationMessageFormat2V3kpmFillIndicationMessageFormat2V3");

            }
            else if(index = -1){
                DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Cell Measurement name, %d", __LINE__);
                return RFAILED;
            }

            ret = kpmFillSliceMeasCondItemV3(measCondItem+cnt, indicationMeasNameFmt2[index], kpmSlicePmDb.snssai[i]);
            if(ret != ROK){
                DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Measurement Conditional item for slice, %d", __LINE__);
                return RFAILED;
            }
            ASN_SEQUENCE_ADD(&indicaMessageFormat2->measData.list, measDataItem+cnt);
            ASN_SEQUENCE_ADD(&indicaMessageFormat2->measCondUEidList.list, measCondItem+cnt);
            cnt = cnt + 1;
        }
    }

    xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format2_v300, indicaMessageFormat2);
    return ROK; 
}

uint8_t kpmFreeMeasInfoList(MeasurementInfoList_t *measInfoList){
    for(int i=0;i<measInfoList->list.count;i++){
        if(measInfoList->list.array[i]->measType.present == MeasurementType_PR_measName){
            DU_FREE(measInfoList->list.array[i]->measType.choice.measName.buf, measInfoList->list.array[i]->measType.choice.measName.size);
        }
        for(int j=0;j<measInfoList->list.array[i]->labelInfoList.list.count;j++){
            kpmFreeMeasLabel(&measInfoList->list.array[i]->labelInfoList.list.array[j]->measLabel);
        }
    }
    return ROK;
}

uint8_t kpmFreeMeasInfoListV3(MeasurementInfoList_v300_t *measInfoList){
    for(int i=0;i<measInfoList->list.count;i++){
        if(measInfoList->list.array[i]->measType.present == MeasurementType_PR_measName){
            DU_FREE(measInfoList->list.array[i]->measType.choice.measName.buf, measInfoList->list.array[i]->measType.choice.measName.size);
        }
        for(int j=0;j<measInfoList->list.array[i]->labelInfoList.list.count;j++){
            kpmFreeMeasLabelV3(&measInfoList->list.array[i]->labelInfoList.list.array[j]->measLabel);
        }
    }
    return ROK;
}

uint8_t kpmFreeRicIndicationMessageFmt1(E2SM_KPM_IndicationMessage_Format1_t *indicaMessageFormat1){
    kpmFreeMeasData(&indicaMessageFormat1->measData);
    if(indicaMessageFormat1->measInfoList){
        kpmFreeMeasInfoList(indicaMessageFormat1->measInfoList);
    }
    if(indicaMessageFormat1->granulPeriod){
        DU_FREE(indicaMessageFormat1->granulPeriod, sizeof(unsigned long));
    }
    return ROK;
}

uint8_t kpmFreeRicIndicationMessageFmt1V3(E2SM_KPM_IndicationMessage_Format1_v300_t *indicaMessageFormat1){
    kpmFreeMeasData(&indicaMessageFormat1->measData);
    if(indicaMessageFormat1->measInfoList){
        kpmFreeMeasInfoListV3(indicaMessageFormat1->measInfoList);
    }
    if(indicaMessageFormat1->granulPeriod){
        DU_FREE(indicaMessageFormat1->granulPeriod, sizeof(unsigned long));
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Fill in the Indication Message
 *
 * @details
 *
 *    Function : kpmFillRicIndicationMessage
 *
 *    Functionality: Fills RICindicationMessage_t to E2AP Handler
 *
 * @param[in] indicaMessageFormat2, E2SM_KPM_IndicationMessage_Format2_v300_t*
 * @param[in] format, int, indicate the Indication Message format
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRicIndicationMessage(RICindicationMessage_t *ricIndicaMsg, int format){
    uint8_t ret = ROK;
    E2SM_KPM_IndicationMessage_t *indicaMessage;
    E2SM_KPM_IndicationMessage_Format1_t *indicaMessageFormat1;
    E2SM_KPM_IndicationMessage_Format2_t *indicaMessageFormat2;

    DU_ALLOC(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_t));
    if(indicaMessage == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Message failed, %d", __LINE__);
        return RFAILED;
    }
    if(format == INDICATION_FORMAT1){
        DU_ALLOC(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
        if(indicaMessageFormat1 == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Message Format 1 failed, %d", __LINE__);
            return RFAILED;
        }
        ret = kpmFillIndicationMessageFormat1(indicaMessageFormat1);  
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Indication Message Format 1, %d", __LINE__);
            DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_t));
            DU_FREE(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
            return RFAILED;
        }
        indicaMessage->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;
        indicaMessage->indicationMessage_formats.choice.indicationMessage_Format1 = indicaMessageFormat1;
    }else if(format == INDICATION_FORMAT2){
        DU_ALLOC(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_t));
        if(indicaMessageFormat2 == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Message Format 2 failed, %d", __LINE__);
            return RFAILED;
        }
        ret = kpmFillIndicationMessageFormat2(indicaMessageFormat2);  
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Indication Message Format 2, %d", __LINE__);
            DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_t));
            DU_FREE(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_t));
            return RFAILED;
        }
        indicaMessage->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format2;
        indicaMessage->indicationMessage_formats.choice.indicationMessage_Format2 = indicaMessageFormat2;
    }else{
        DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Indication Message Format, %d", __LINE__);
        return RFAILED;
    }
    
    uint8_t e2smbuffer[8192] = {0, };
    size_t e2smbuffer_size = 8192;
    asn_codec_ctx_t *opt_cod = 0;

    asn_enc_rval_t er =
    asn_encode_to_buffer(opt_cod,
         ATS_ALIGNED_BASIC_PER,
         &asn_DEF_E2SM_KPM_IndicationMessage,
         indicaMessage, e2smbuffer, e2smbuffer_size);

    if(er.encoded == -1){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not encode Indication Message, %d", __LINE__);
        DU_LOG("Failed to encode element %s", er.failed_type ? er.failed_type->name : ""); 
        if(format == INDICATION_FORMAT1){
            kpmFreeRicIndicationMessageFmt1(indicaMessageFormat1);
            DU_FREE(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
        }
        else if(format == INDICATION_FORMAT2){
            kpmFreeRicIndicationMessageFmt2(indicaMessageFormat2);
            DU_FREE(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_t));
        }
        DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_t));
        return RFAILED;
    }
    else{
        DU_ALLOC(ricIndicaMsg->buf, er.encoded * sizeof(uint8_t));
        if(ricIndicaMsg->buf == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RIC Indication Message failed, %d", __LINE__);
            return RFAILED;
        }
        ricIndicaMsg->size = er.encoded;
        memcpy(ricIndicaMsg->buf,e2smbuffer,er.encoded);
    }
    if(format == INDICATION_FORMAT1){
        kpmFreeRicIndicationMessageFmt1(indicaMessageFormat1);
        DU_FREE(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
    }
    else if(format == INDICATION_FORMAT2){
        kpmFreeRicIndicationMessageFmt2(indicaMessageFormat2);
        DU_FREE(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_t));
    }
    DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_t));
    return ROK; 
}

/*******************************************************************
 *
 * @brief Fill in the Indication Message of KPMv3
 *
 * @details
 *
 *    Function : kpmFillRicIndicationMessage
 *
 *    Functionality: Fills RICindicationMessage_t to E2AP Handler
 *
 * @param[in] indicaMessageFormat2, E2SM_KPM_IndicationMessage_Format2_v300_t*
 * @param[in] format, int, indicate the Indication Message format
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFillRicIndicationMessageV3(RICindicationMessage_t *ricIndicaMsg, int format){
    uint8_t ret = ROK;
    E2SM_KPM_IndicationMessage_v300_t *indicaMessage;
    E2SM_KPM_IndicationMessage_Format1_v300_t *indicaMessageFormat1;
    E2SM_KPM_IndicationMessage_Format2_v300_t *indicaMessageFormat2;

    DU_ALLOC(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_v300_t));
    if(indicaMessage == NULLP){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Message failed, %d", __LINE__);
        return RFAILED;
    }
    if(format == INDICATION_FORMAT1){
        DU_ALLOC(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_v300_t));
        if(indicaMessageFormat1 == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Message Format 1 failed, %d", __LINE__);
            return RFAILED;
        }
        ret = kpmFillIndicationMessageFormat1V3(indicaMessageFormat1);  
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Indication Message Format 1, %d", __LINE__);
            DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_v300_t));
            DU_FREE(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_v300_t));
            return RFAILED;
        }
        indicaMessage->indicationMessage_formats.present = E2SM_KPM_IndicationMessage_v300__indicationMessage_formats_PR_indicationMessage_Format1;
        indicaMessage->indicationMessage_formats.choice.indicationMessage_Format1 = indicaMessageFormat1;
    }else if(format == INDICATION_FORMAT2){
        DU_ALLOC(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_v300_t));
        if(indicaMessageFormat2 == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for Indication Message Format 1 failed, %d", __LINE__);
            return RFAILED;
        }
        ret = kpmFillIndicationMessageFormat2V3(indicaMessageFormat2);  
        if(ret != ROK){
            DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill Indication Message Format 1, %d", __LINE__);
            DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_v300_t));
            DU_FREE(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_v300_t));
            return RFAILED;
        }
        indicaMessage->indicationMessage_formats.present = E2SM_KPM_IndicationMessage_v300__indicationMessage_formats_PR_indicationMessage_Format2;
        indicaMessage->indicationMessage_formats.choice.indicationMessage_Format2 = indicaMessageFormat2;
    }else{
        DU_LOG("\nERROR   -->  E2SM-KPM : Invalid Indication Message Format, %d", __LINE__);
        return RFAILED;
    }

    uint8_t e2smbuffer[8192] = {0, };
    size_t e2smbuffer_size = 8192;
    asn_codec_ctx_t *opt_cod = 0;

    asn_enc_rval_t er =
    asn_encode_to_buffer(opt_cod,
         ATS_ALIGNED_BASIC_PER,
         &asn_DEF_E2SM_KPM_IndicationMessage_v300,
         indicaMessage, e2smbuffer, e2smbuffer_size);

    if(er.encoded == -1){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not encode Indication Message, %d", __LINE__);
        DU_LOG("Failed to encode element %s", er.failed_type ? er.failed_type->name : ""); 
        if(format == INDICATION_FORMAT1){
            kpmFreeRicIndicationMessageFmt1V3(indicaMessageFormat1);
            DU_FREE(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_v300_t));
        }
        else if(format == INDICATION_FORMAT2){
            kpmFreeRicIndicationMessageFmt2V3(indicaMessageFormat2);
            DU_FREE(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_v300_t));
        }
        DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_v300_t));
        return RFAILED;
    }
    else{
        DU_ALLOC(ricIndicaMsg->buf, er.encoded * sizeof(uint8_t));
        if(ricIndicaMsg->buf == NULLP){
            DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RIC Indication Message failed, %d", __LINE__);
            return RFAILED;
        }
        ricIndicaMsg->size = er.encoded;
        memcpy(ricIndicaMsg->buf,e2smbuffer,er.encoded);
    }
    if(format == INDICATION_FORMAT1){
        kpmFreeRicIndicationMessageFmt1V3(indicaMessageFormat1);
        DU_FREE(indicaMessageFormat1, sizeof(E2SM_KPM_IndicationMessage_Format1_v300_t));
    }
    else if(format == INDICATION_FORMAT2){
        kpmFreeRicIndicationMessageFmt2V3(indicaMessageFormat2);
        DU_FREE(indicaMessageFormat2, sizeof(E2SM_KPM_IndicationMessage_Format2_v300_t));
    }
    DU_FREE(indicaMessage, sizeof(E2SM_KPM_IndicationMessage_v300_t));
    return ROK; 
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for MeasurementData_t 
 *
 * @details
 *
 *    Function : kpmFreeMeasData
 *
 *    Functionality: freeing the memory allocated for MeasurementData_t
 *
 * @param[in] measData, MeasurementData_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeMeasData(MeasurementData_t *measData){
    for(int i=0;i<measData->list.count;i++){
        if(measData->list.array[i] != NULL){
            if(measData->list.array[i]->incompleteFlag){
                DU_FREE(measData->list.array[i]->incompleteFlag, sizeof(long));
            }

            for(int j=0;j<measData->list.array[i]->measRecord.list.count;j++){
                if(measData->list.array[i]->measRecord.list.array[j] != NULL){
                    DU_FREE(measData->list.array[i]->measRecord.list.array[j], sizeof(MeasurementRecordItem_t));
                }
            }
        }
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for MeasurementLabel_t 
 *
 * @details
 *
 *    Function : kpmFreeMeasLabel
 *
 *    Functionality: freeing the memory allocated for MeasurementLabel_t
 *
 * @param[in] measLabel, MeasurementLabel_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeMeasLabel(MeasurementLabel_t *measLabel){
    if(measLabel->noLabel){
        DU_FREE(measLabel->noLabel, sizeof(long));
    }
    if(measLabel->plmnID){
        kpmFreeOctetString(measLabel->plmnID);
    }
    if(measLabel->sliceID){
        if(measLabel->sliceID->sD){
            kpmFreeOctetString(measLabel->sliceID->sD);
        }
        if(measLabel->sliceID->sST.size>0){
            DU_FREE(measLabel->sliceID->sST.buf, measLabel->sliceID->sST.size);
        }
    }
    if(measLabel->fiveQI){
        DU_FREE(measLabel->fiveQI, sizeof(long));
    }
    if(measLabel->qFI){
        DU_FREE(measLabel->qFI, sizeof(long));
    }
    if(measLabel->qCI){
        DU_FREE(measLabel->qCI, sizeof(long));   
    }
    if(measLabel->qCImax){
        DU_FREE(measLabel->qCImax, sizeof(long));
    }
    if(measLabel->qCImin){
        DU_FREE(measLabel->qCImin, sizeof(long));
    }
    if(measLabel->aRPmax){
        DU_FREE(measLabel->aRPmax, sizeof(long));
    }
    if(measLabel->aRPmin){
        DU_FREE(measLabel->aRPmin, sizeof(long));
    }
    if(measLabel->bitrateRange){
        DU_FREE(measLabel->bitrateRange, sizeof(long));
    }
    if(measLabel->layerMU_MIMO){
        DU_FREE(measLabel->layerMU_MIMO, sizeof(long));
    }
    if(measLabel->sUM){
        DU_FREE(measLabel->sUM, sizeof(long));
    }
    if(measLabel->distBinX){
        DU_FREE(measLabel->distBinX, sizeof(long));
    }
    if(measLabel->distBinY){
        DU_FREE(measLabel->distBinY, sizeof(long));
    }
    if(measLabel->distBinZ){
        DU_FREE(measLabel->distBinZ, sizeof(long));
    }
    if(measLabel->min){
        DU_FREE(measLabel->min, sizeof(long));
    }
    if(measLabel->max){
        DU_FREE(measLabel->max, sizeof(long));
    }
    if(measLabel->avg){
        DU_FREE(measLabel->avg, sizeof(long));
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for MeasurementLabel_v300_t of KPMv3
 *
 * @details
 *
 *    Function : kpmFreeMeasLabel
 *
 *    Functionality: freeing the memory allocated for MeasurementLabel_v300_t
 *
 * @param[in] measLabel, MeasurementLabel_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeMeasLabelV3(MeasurementLabel_v300_t *measLabel){
    if(measLabel->noLabel){
        DU_FREE(measLabel->noLabel, sizeof(long));
    }
    if(measLabel->plmnID){
        kpmFreeOctetString(measLabel->plmnID);
    }
    if(measLabel->sliceID){
        if(measLabel->sliceID->sD){
            kpmFreeOctetString(measLabel->sliceID->sD);
        }
        if(measLabel->sliceID->sST.size>0){
            DU_FREE(measLabel->sliceID->sST.buf, measLabel->sliceID->sST.size);
        }
    }
    if(measLabel->fiveQI){
        DU_FREE(measLabel->fiveQI, sizeof(long));
    }
    if(measLabel->qFI){
        DU_FREE(measLabel->qFI, sizeof(long));
    }
    if(measLabel->qCI){
        DU_FREE(measLabel->qCI, sizeof(long));   
    }
    if(measLabel->qCImax){
        DU_FREE(measLabel->qCImax, sizeof(long));
    }
    if(measLabel->qCImin){
        DU_FREE(measLabel->qCImin, sizeof(long));
    }
    if(measLabel->aRPmax){
        DU_FREE(measLabel->aRPmax, sizeof(long));
    }
    if(measLabel->aRPmin){
        DU_FREE(measLabel->aRPmin, sizeof(long));
    }
    if(measLabel->bitrateRange){
        DU_FREE(measLabel->bitrateRange, sizeof(long));
    }
    if(measLabel->layerMU_MIMO){
        DU_FREE(measLabel->layerMU_MIMO, sizeof(long));
    }
    if(measLabel->sUM){
        DU_FREE(measLabel->sUM, sizeof(long));
    }
    if(measLabel->distBinX){
        DU_FREE(measLabel->distBinX, sizeof(long));
    }
    if(measLabel->distBinY){
        DU_FREE(measLabel->distBinY, sizeof(long));
    }
    if(measLabel->distBinZ){
        DU_FREE(measLabel->distBinZ, sizeof(long));
    }
    if(measLabel->min){
        DU_FREE(measLabel->min, sizeof(long));
    }
    if(measLabel->max){
        DU_FREE(measLabel->max, sizeof(long));
    }
    if(measLabel->avg){
        DU_FREE(measLabel->avg, sizeof(long));
    }
    if(measLabel->ssbIndex){
        DU_FREE(measLabel->ssbIndex, sizeof(long));
    }
    if(measLabel->ssbIndex){
        DU_FREE(measLabel->nonGoB_BFmode_Index, sizeof(long));
    }
    if(measLabel->ssbIndex){
        DU_FREE(measLabel->mIMO_mode_Index, sizeof(long));
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for MeasurementCondUEidList_t
 *
 * @details
 *
 *    Function : kpmFreeCondUEidList
 *
 *    Functionality: freeing the memory allocated for MeasurementCondUEidList_t
 *
 * @param[in] measCondUEidList, MeasurementCondUEidList_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeCondUEidList(MeasurementCondUEidList_t *measCondUEidList){
    MeasurementLabel_t *measLabel;
    for(int i=0;i<measCondUEidList->list.count;i++){
        if(measCondUEidList->list.array[i] != NULL){
            if(measCondUEidList->list.array[i]->measType.present == MeasurementType_PR_measName){
                DU_FREE(measCondUEidList->list.array[i]->measType.choice.measName.buf, measCondUEidList->list.array[i]->measType.choice.measName.size);
            }
            for(int j=0;j<measCondUEidList->list.array[i]->matchingCond.list.count;j++){
                if(measCondUEidList->list.array[i]->matchingCond.list.array[j]){
                    switch(measCondUEidList->list.array[i]->matchingCond.list.array[j]->present){
                        case MatchingCondItem_PR_measLabel:
                            measLabel = measCondUEidList->list.array[i]->matchingCond.list.array[j]->choice.measLabel;
                            kpmFreeMeasLabel(measLabel);
                        break;
                        case MatchingCondItem_PR_testCondInfo:
                            DU_LOG("\nERROR   -->  E2SM-KPM : Freeing testCondInfo is not supported");
                        default:
                            DU_LOG("\nERROR   -->  E2SM-KPM : kpmFreeCondUEidList type is invalid");
                        break;
                    }
                }
            }
            if(measCondUEidList->list.array[i]->matchingUEidList){
                DU_LOG("\nERROR   -->  E2SM-KPM : Free matchingUEidList is not supported");
            }

        }
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for MeasurementCondUEidList_v300_t of KPMv3
 *
 * @details
 *
 *    Function : kpmFreeCondUEidListV3
 *
 *    Functionality: freeing the memory allocated for MeasurementCondUEidList_v300_t
 *
 * @param[in] measCondUEidList, MeasurementCondUEidList_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeCondUEidListV3(MeasurementCondUEidList_v300_t *measCondUEidList){
    MeasurementLabel_v300_t *measLabel;
    for(int i=0;i<measCondUEidList->list.count;i++){
        if(measCondUEidList->list.array[i] != NULL){
            if(measCondUEidList->list.array[i]->measType.present == MeasurementType_PR_measName){
                DU_FREE(measCondUEidList->list.array[i]->measType.choice.measName.buf, measCondUEidList->list.array[i]->measType.choice.measName.size);
            }
            for(int j=0;j<measCondUEidList->list.array[i]->matchingCond.list.count;j++){
                if(measCondUEidList->list.array[i]->matchingCond.list.array[j]){
                    switch(measCondUEidList->list.array[i]->matchingCond.list.array[j]->matchingCondChoice.present){
                        case MatchingCondItem_Choice_PR_measLabel:
                            measLabel = measCondUEidList->list.array[i]->matchingCond.list.array[j]->matchingCondChoice.choice.measLabel;
                            kpmFreeMeasLabelV3(measLabel);
                        break;
                        case MatchingCondItem_Choice_PR_testCondInfo:
                            DU_LOG("\nERROR   -->  E2SM-KPM : Freeing testCondInfo is not supported");
                        default:
                            DU_LOG("\nERROR   -->  E2SM-KPM : kpmFreeCondUEidListV3 type is invalid");
                        break;
                    }
                }
            }
            if(measCondUEidList->list.array[i]->matchingUEidList){
                DU_LOG("\nERROR   -->  E2SM-KPM : Free matchingUEidList is not supported");
            }
        }
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_IndicationMessage_Format2_t
 *
 * @details
 *
 *    Function : kpmFreeRicIndicationMessageFmt2
 *
 *    Functionality: freeing the memory allocated for E2SM_KPM_IndicationMessage_Format2_t
 *
 * @param[in] indicaMessageFormat2, E2SM_KPM_IndicationMessage_Format2_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeRicIndicationMessageFmt2(E2SM_KPM_IndicationMessage_Format2_t *indicaMessageFormat2){
    kpmFreeMeasData(&indicaMessageFormat2->measData);
    kpmFreeCondUEidList(&indicaMessageFormat2->measCondUEidList);
    if(indicaMessageFormat2->granulPeriod){
        DU_FREE(indicaMessageFormat2->granulPeriod, sizeof(unsigned long));
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for E2SM_KPM_IndicationMessage_Format2_v300_t
 *
 * @details
 *
 *    Function : kpmFreeRicIndicationMessageFmt2V3
 *
 *    Functionality: freeing the memory allocated for E2SM_KPM_IndicationMessage_Format2_v300_t
 *
 * @param[in] indicaMessageFormat2, E2SM_KPM_IndicationMessage_Format2_v300_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeRicIndicationMessageFmt2V3(E2SM_KPM_IndicationMessage_Format2_v300_t *indicaMessageFormat2){
    kpmFreeMeasData(&indicaMessageFormat2->measData);
    kpmFreeCondUEidListV3(&indicaMessageFormat2->measCondUEidList);
    if(indicaMessageFormat2->granulPeriod){
        DU_FREE(indicaMessageFormat2->granulPeriod, sizeof(unsigned long));
    }
    return ROK;
}

/*******************************************************************
 *
 * @brief Deallocate the memory allocated for OCTET_STRING_t
 *
 * @details
 *
 *    Function : kpmFreeOctetString
 *
 *    Functionality: freeing the memory allocated for OCTET_STRING_t
 *
 * @param[in] octetString, OCTET_STRING_t*
 * @return ROK     - success
 *         RFAILED - failure
 *
 ******************************************************************/

uint8_t kpmFreeOctetString(OCTET_STRING_t *octetString){
    if(octetString){
        if(octetString->size > 0){
            if(octetString->buf){
                DU_FREE(octetString->buf, octetString->size);
            }
        }
        DU_FREE(octetString, sizeof(OCTET_STRING_t));
        octetString = NULL;
        return ROK;
    }
    else{
        return RFAILED;
    }

}

/*******************************************************************
 *
 * @brief Send the slice metrics to RIC with RIC Indication
 *
 * @details
 *
 *    Function : kpmSendSliceMetric
 *
 *    Functionality: Build the slice metrics and send RIC Indication to RIC
 *
 * @return void
 *
 ******************************************************************/

void kpmSendSliceMetric(){
    RICindicationHeader_t *ricIndicationHeader;
    RICindicationMessage_t *ricIndicationMessage;
    uint8_t ret;
    if(kpmSliceIndicationEnable == false){
        DU_LOG("\nINFO   -->  E2SM-KPM : Send Slice Metric is disabled, %d", __LINE__);
        return;
        
    }
    DU_ALLOC(ricIndicationMessage, sizeof(RICindicationMessage_t));
    if(ricIndicationMessage == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RIC Indication Message failed, %d", __LINE__);
        return;
    }

    DU_ALLOC(ricIndicationHeader, sizeof(RICindicationHeader_t));
    if(ricIndicationHeader == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RIC Indication Header failed, %d", __LINE__);
        return;
    }

    kpmCalcSliceMetric();
    if(kpmIndicationV3Enable){
        ret = kpmFillRicIndicationHeaderV3(ricIndicationHeader);
    }
    else{
        DU_LOG("\nINFO   -->  E2SM-KPM : Sending Slice Metric, %d", __LINE__);
        ret = kpmFillRicIndicationHeader(ricIndicationHeader);
    }
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill RIC Indication Header, %d", __LINE__);
        return;
    }
    if(kpmIndicationV3Enable){
        ret = kpmFillRicIndicationMessageV3(ricIndicationMessage, INDICATION_FORMAT2);
    }
    else{
        ret = kpmFillRicIndicationMessage(ricIndicationMessage, INDICATION_FORMAT2);
    }
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill RIC Indication Message, %d", __LINE__);
        return;
    }
    BuildAndSendRicIndication(ricIndicationHeader, ricIndicationMessage);
}

/*******************************************************************
 *
 * @brief Send the cell metrics to RIC with RIC Indication
 *
 * @details
 *
 *    Function : kpmSendCellMetric
 *
 *    Functionality: Build the cell metrics and send RIC Indication to RIC
 *
 * @return void
 *
 ******************************************************************/

void kpmSendCellMetric(){
    RICindicationHeader_t *ricIndicationHeader;
    RICindicationMessage_t *ricIndicationMessage;
    uint8_t ret;
    if(kpmCellIndicationEnable == false){
        return;
    }
    DU_ALLOC(ricIndicationMessage, sizeof(RICindicationMessage_t));
    if(ricIndicationMessage == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RIC Indication Message failed, %d", __LINE__);
        return;
    }

    DU_ALLOC(ricIndicationHeader, sizeof(RICindicationHeader_t));
    if(ricIndicationHeader == NULL){
        DU_LOG("\nERROR   -->  E2SM-KPM : Memory allocation for RIC Indication Header failed, %d", __LINE__);
        return;
    }

    kpmCalcCellMetric();
    if(kpmIndicationV3Enable){
        ret = kpmFillRicIndicationHeaderV3(ricIndicationHeader);
    }
    else{
        ret = kpmFillRicIndicationHeader(ricIndicationHeader);
    }
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill RIC Indication Header, %d", __LINE__);
        return;
    }
    if(kpmIndicationV3Enable){
        ret = kpmFillRicIndicationMessageV3(ricIndicationMessage, INDICATION_FORMAT1);
    }
    else{
        ret = kpmFillRicIndicationMessage(ricIndicationMessage, INDICATION_FORMAT1);
    }
    if(ret != ROK){
        DU_LOG("\nERROR   -->  E2SM-KPM : Could not fill RIC Indication Message, %d", __LINE__);
        return;
    }
    BuildAndSendRicIndication(ricIndicationHeader, ricIndicationMessage);
}

/*******************************************************************
 *
 * @brief Send the metrics to RIC with RIC Indication
 *
 * @details
 *
 *    Function : kpmSendMetrics
 *
 *    Functionality: Build and send RIC Indication to RIC
 *
 * @return void
 *
 ******************************************************************/

void kpmSendMetrics(){

    kpmSendCellMetric();
    kpmSendSliceMetric();
}
