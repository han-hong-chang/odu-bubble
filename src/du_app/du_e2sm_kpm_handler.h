#ifndef _KPM_H_
#define _KPM_H_
#include "common_def.h"
#include "lrg.h"
#include "lkw.x"
#include "lrg.x"
#include "legtp.h"
#include "du_app_mac_inf.h"
#include "du_app_rlc_inf.h"
#include "du_cfg.h"
#include "du_mgr_main.h"
#include "du_mgr.h"
#include "du_utils.h"
#include "E2SM-KPM-RANfunction-Description.h"
#include "RIC-EventTriggerStyle-Item.h"
#include "RIC-ReportStyle-Item.h"
#include "MeasurementInfo-Action-Item.h"
#include "ProtocolIE-Field.h"
#include "RANfunction-Item.h"
#include "E2SM-KPM-ActionDefinition.h"
#include "E2SM-KPM-ActionDefinition-Format1.h"
#include "E2SM-KPM-ActionDefinition-Format3.h"
#include "MeasurementDataItem.h"
#include "E2SM-KPM-IndicationMessage.h"
#include "E2SM-KPM-IndicationMessage-Format1.h"
#include "E2SM-KPM-IndicationMessage-Format2.h"
#include "E2SM-KPM-IndicationHeader.h"
#include "E2SM-KPM-IndicationHeader-Format1.h"
#include "TimeStamp.h"
#include "constr_TYPE.h"
#include "OCTET_STRING.h"
#include "asn_application.h"
#include "E2SM-KPM-IndicationMessage.h"
#include "E2SM-KPM-IndicationHeader-Format1.h"
#include "E2SM-KPM-EventTriggerDefinition.h"
#include "E2SM-KPM-EventTriggerDefinition-Format1.h"
#include "E2SM-KPM-IndicationHeader.h"
#include "MeasurementInfoItem.h"
#include "MeasurementCondItem.h"
#include "LabelInfoItem.h"
#include "E2AP-PDU.h"
#include "RICsubscriptionRequest.h"
#include "RICsubscriptionResponse.h"
#include "RICactionType.h"
#include "ProtocolIE-SingleContainer.h"
#include "InitiatingMessage.h"
#include "E2setupRequest.h"
#include "RANfunctionOID.h"
#include "GlobalE2node-gNB-ID.h"
#include "ProtocolIE-FieldE2.h"
#include "InitiatingMessageE2.h"
#include "SuccessfulOutcomeE2.h"
#include "odu_common_codec.h"
#include "E2nodeComponentInterfaceF1.h"
#include "MeasurementRecordItem.h"
#include "MeasurementCondUEidList.h"
#include "MeasurementCondUEidItem.h"
#include "MatchingCondItem.h"
#include "du_e2ap_msg_hdl.h"
#include "du_app_mac_inf.h"
#include "MatchingUEidList.h"
#include "du_kpi_module.h"

// Extend for KPM v3
#include "E2SM-KPM-RANfunction-Description-v300.h"
#include "RIC-ReportStyle-Item-v300.h"
#include "MeasurementInfo-Action-Item-v300.h"
#include "E2SM-KPM-ActionDefinition-v300.h"
#include "E2SM-KPM-ActionDefinition-Format1-v300.h"
#include "E2SM-KPM-ActionDefinition-Format3-v300.h"
#include "E2SM-KPM-IndicationMessage-v300.h"
#include "E2SM-KPM-IndicationMessage-Format1-v300.h"
#include "E2SM-KPM-IndicationMessage-Format2-v300.h"
#include "MeasurementInfoItem-v300.h"
#include "MeasurementCondItem-v300.h"
#include "LabelInfoItem-v300.h"
#include "MeasurementCondUEidList-v300.h"
#include "MeasurementCondUEidItem-v300.h"
#include "MatchingCondItem-v300.h"
#include "E2SM-KPM-IndicationHeader-v300.h"
#include "E2SM-KPM-IndicationHeader-Format1-v300.h"


#define INDICATION_MSG_FMRT1 1
#define INDICATION_MSG_FMRT2 1
#define NUM_OF_CELL_METRIC 4
#define NUM_OF_SLICE_METRIC 2
#define SUPPORTED_CELL_MEAS_ITEM 4
#define SUPPORTED_SLICE_MEAS_ITEM 2
#define MAX_OF_MEAS_NAME 25
#define INDICATION_FORMAT1 1
#define INDICATION_FORMAT2 2
#define SIZE_OF_MAX_INDICATION_INFO 4

uint8_t kpmInit();
uint8_t kpmEnableIndication(bool *flag);
uint8_t kpmFillOctetString(OCTET_STRING_t *dst, uint8_t *src);
uint8_t kpmFillRanFunctionName(RANfunction_Name_t *ranfunc_name, uint8_t kpmVersion);
uint8_t kpmFillTriggerStyleItem(RIC_EventTriggerStyle_Item_t *trigger_style);
uint8_t kpmFillTriggerStyleList(struct E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List *triggerStyleList);
uint8_t kpmFillTriggerStyleListV3(struct E2SM_KPM_RANfunction_Description_v300__ric_EventTriggerStyle_List *triggerStyleList);
uint8_t kpmFillReportStyleItemStyle1(RIC_ReportStyle_Item_t *report_style);
uint8_t kpmFillReportStyleItemStyle1V3(RIC_ReportStyle_Item_v300_t *report_style);
uint8_t kpmFillReportStyleItemStyle2(RIC_ReportStyle_Item_t *report_style);
uint8_t kpmFillReportStyleItemStyle2V3(RIC_ReportStyle_Item_v300_t *report_style);
uint8_t kpmFillReportStyleList(struct E2SM_KPM_RANfunction_Description__ric_ReportStyle_List *reportStyleList);
uint8_t kpmFreeRanFuncName(RANfunction_Name_t *ranfunc_name);
uint8_t kpmFreeEventTrigStyleList(struct E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List *eventTrigStyleList);
uint8_t kpmFreeReportStyleList(struct E2SM_KPM_RANfunction_Description__ric_ReportStyle_List *reportStyleList);
uint8_t kpmFreeRanFuncDesc(E2SM_KPM_RANfunction_Description_t *ranfunc_desc);
uint8_t kpmFillRanFuncDescription(RANfunctionDefinition_t  *ranFunDefinition);
uint8_t kpmFillRanFuncDescriptionV3(RANfunctionDefinition_t  *ranFunDefinition);
uint8_t kpmFillMeasInfoItem(MeasurementInfoItem_t *measInfoItem, uint8_t *measName, int size);
uint8_t kpmProcActionDefinitionFmt1(E2SM_KPM_ActionDefinition_Format1_t *actionDefinFormat1);
uint8_t kpmProcActionDefinitionFmt3(E2SM_KPM_ActionDefinition_Format3_t *actionDefinFormat3);
uint8_t kpmProcEventTrigDefinitionFormat1(RICeventTriggerDefinition_t *defini);
int kpmCellMeasNameIsSupport(uint8_t *measNameBuf);
int kpmIsSupport(uint8_t *measNameBuf);
uint8_t kpmProcActionDefinition(RICactionDefinition_t *ricdifin);
uint8_t kpmProcActionDefinitionFmt1V3(E2SM_KPM_ActionDefinition_Format1_v300_t *actionDefinFormat1);
uint8_t kpmProcActionDefinitionFmt3V3(E2SM_KPM_ActionDefinition_Format3_v300_t *actionDefinFormat3);
uint8_t kpmProcActionDefinitionV3(RICactionDefinition_t *ricdifin);
uint8_t kpmFreeIndicationHeader(E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1);
uint8_t kpmFillIndicationHeaderFormat1Optional(E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1);
uint8_t kpmFillIndicationHeaderFormat1(E2SM_KPM_IndicationHeader_Format1_t *indicaHeaderFormat1);
uint8_t kpmFillRicIndicationHeader(RICindicationHeader_t *ricIndicationHeader);
uint8_t kpmFillIndicationHeaderFormat1OptionalV3(E2SM_KPM_IndicationHeader_Format1_v300_t *indicaHeaderFormat1);
uint8_t kpmFillIndicationHeaderFormat1V3(E2SM_KPM_IndicationHeader_Format1_v300_t *indicaHeaderFormat1);
uint8_t kpmFillRicIndicationHeaderV3(RICindicationHeader_t *ricIndicationHeader);
uint8_t kpmFillSliceMeasDataItem(MeasurementDataItem_t *measDataItem, unsigned long intValue);
uint8_t kpmFillCellMeasDataItem(MeasurementDataItem_t *item, MeasurementRecordItem_PR present, int integer, double real);
uint8_t kpmFillIndicationMessageFormat1(E2SM_KPM_IndicationMessage_Format1_t *indicaMessageFormat1);
uint8_t kpmFillIndicationMessageFormat1V3(E2SM_KPM_IndicationMessage_Format1_v300_t *indicaMessageFormat1);
uint8_t kpmFillSliceMeasCondItem(MeasurementCondUEidItem_t *measCondItem, char measureName[], Snssai snssai);
uint8_t kpmFillSliceMeasCondItemV3(MeasurementCondUEidItem_v300_t *measCondItem, char measureName[], Snssai snssai);
uint8_t kpmFillIndicationMessageFormat2(E2SM_KPM_IndicationMessage_Format2_t *indicaMessageFormat2);
uint8_t kpmFillIndicationMessageFormat2V3(E2SM_KPM_IndicationMessage_Format2_v300_t *indicaMessageFormat2);
uint8_t kpmFreeMeasInfoList(MeasurementInfoList_t *measInfoList);
uint8_t kpmFreeRicIndicationMessageFmt1(E2SM_KPM_IndicationMessage_Format1_t *indicaMessageFormat1);
uint8_t kpmFreeRicIndicationMessageFmt1V3(E2SM_KPM_IndicationMessage_Format1_v300_t *indicaMessageFormat1);
uint8_t kpmFillRicIndicationMessage(RICindicationMessage_t *ricIndicaMsg, int format);
uint8_t kpmFillRicIndicationMessageV3(RICindicationMessage_t *ricIndicaMsg, int format);
uint8_t kpmFreeMeasData(MeasurementData_t *measData);
uint8_t kpmFreeMeasLabel(MeasurementLabel_t *measLabel);
uint8_t kpmFreeMeasLabelV3(MeasurementLabel_v300_t *measLabel);
uint8_t kpmFreeMeasInfoListV3(MeasurementInfoList_v300_t *measInfoList);
uint8_t kpmFreeCondUEidList(MeasurementCondUEidList_t *measCondUEidList);
uint8_t kpmFreeRicIndicationMessageFmt2(E2SM_KPM_IndicationMessage_Format2_t *indicaMessageFormat2);
uint8_t kpmFreeRicIndicationMessageFmt2V3(E2SM_KPM_IndicationMessage_Format2_v300_t *indicaMessageFormat2);
uint8_t kpmFreeOctetString(OCTET_STRING_t *octetString);
void kpmSendSliceMetric();
void kpmSendCellMetric();
void kpmSendMetrics();


bool kpmIndicationV3Enable;

uint32_t reportingPeriodFmt1; // For RIC Indication format 1 (Cell)
uint32_t reportingPeriodFmt2; // For RIC Indication format 2 (Slice)
uint32_t reportingPeriod; // Store the reporting period of Event Trigger Definition
uint8_t indicationMeasNameFmt1[SIZE_OF_MAX_INDICATION_INFO][MAX_OF_MEAS_NAME]; // Subscribed Measurement name for indication format 1
uint8_t indicationMeasNameFmt2[SIZE_OF_MAX_INDICATION_INFO][MAX_OF_MEAS_NAME]; // Subscribed Measurement name for indication format 2
uint8_t cellMetricName[SUPPORTED_CELL_MEAS_ITEM][MAX_OF_MEAS_NAME]; // Support cell metric in O-DU
uint8_t sliceMetricName[SUPPORTED_SLICE_MEAS_ITEM][MAX_OF_MEAS_NAME]; // Support slice metric in O-DU
int sizeOfMeasNameFmt1; // Size of Subscribed measurement for indication format 1
int sizeOfMeasNameFmt3; // Size of Subscribed measurement for indication format 2

#endif
