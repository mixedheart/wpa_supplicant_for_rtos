#ifndef MEMCTRL_H
#define MEMCTEL_H                            
/* define controller bit for MEMCTL1_REG_2 */
#define MEMCTL1_REG_2_INITAREF							24  
#define MEMCTL1_REG_2_TINIT								0 

/* define controller bit for MEMCTL1_REG_3 */
#define MEMCTL1_REG_3_TCCD								24
#define MEMCTL1_REG_3_MUST_BE_1							16
#define MEMCTL1_REG_3_CASLAT_LIN						0

/* define controller bit for MEMCTL1_REG_4 */
#define MEMCTL1_REG_4_TRAS_MIN							24
#define MEMCTL1_REG_4_TRC								16
#define MEMCTL1_REG_4_TRCD								8
#define MEMCTL1_REG_4_TBST_INT_INTERVAL					0

/* define controller bit for MEMCTL1_REG_5 */
#define MEMCTL1_REG_5_TMRD								24
#define MEMCTL1_REG_5_TRP								8
#define MEMCTL1_REG_5_TWTR								0

/* define controller bit for MEMCTL1_REG_6 */
#define MEMCTL1_REG_6_WRITEINTERP						24
#define MEMCTL1_REG_6_TRAS_MAX							8
#define MEMCTL1_REG_6_TMOD								0

/* define controller bit for MEMCTL1_REG_7 */
#define MEMCTL1_REG_7_CONCURRENTAP						24
#define MEMCTL1_REG_7_AP								16
#define MEMCTL1_REG_7_TCKESR							8
#define MEMCTL1_REG_7_TCKE								0

/* define controller bit for MEMCTL1_REG_8 */
#define MEMCTL1_REG_8_TDAL								24
#define MEMCTL1_REG_8_TWR								16
#define MEMCTL1_REG_8_TRCD_INT							8
#define MEMCTL1_REG_8_TRAS_LOCKOUT						0
                                                    	
/* define controller bit for MEMCTL1_REG_9 */       	
#define MEMCTL1_REG_9_BURST_BSTLEN						24
                                                    	
/* define controller bit for MEMCTL1_REG_10 */      	
#define MEMCTL1_REG_10_TRP_AB							24
#define MEMCTL1_REG_10_TFAW								0
                                                    	
/* define controller bit for MEMCTL1_REG_11 */      	
#define MEMCTL1_REG_11_TREF_ENABLE						24
#define MEMCTL1_REG_11_AUTO_REFRESH_MODE				16
#define MEMCTL1_REG_11_AREFRESH					8
                                                    	
/* define controller bit for MEMCTL1_REG_12 */      	
#define MEMCTL1_REG_12_TREFI							16
#define MEMCTL1_REG_12_TRFC								0
                                                    	
/* define controller bit for MEMCTL1_REG_13 */      	
#define MEMCTL1_REG_13_TXP								8
                                                    	
/* define controller bit for MEMCTL1_REG_14 */      	
#define MEMCTL1_REG_14_TXSNR							16
#define MEMCTL1_REG_14_TXSR								0

/* define controller bit for MEMCTL1_REG_15 */
#define MEMCTL1_REG_15_CKE_DELAY						24
#define MEMCTL1_REG_15_ENABLE_QUICK_SREFRESH			16
#define MEMCTL1_REG_15_SREFRESH							0

/* define controller bit for MEMCTL1_REG_16 */
#define MEMCTL1_REG_16_LP_MODE1_2_CNT					8
#define MEMCTL1_REG_16_LP_CONTROL						0

/* define controller bit for MEMCTL1_REG_17 */
#define MEMCTL1_REG_17_LP_MODE4_CNT						16
#define MEMCTL1_REG_17_LP_MODE3_CNT						0

/* define controller bit for MEMCTL1_REG_18 */
#define MEMCTL1_REG_18_LP_MODE5_CNT						8
#define MEMCTL1_REG_18_LP_AUTO_ENABLE					0

/* define controller bit for MEMCTL1_REG_19 */
#define MEMCTL1_REG_19_CKSRE							16
#define MEMCTL1_REG_19_LP_REFRESH_ENABLE				0

/* define controller bit for MEMCTL1_REG_20 */
#define MEMCTL1_REG_20_MR_DATA_0						16
#define MEMCTL1_REG_20_WRITE_MODEREG					8
#define MEMCTL1_REG_20_CKSRX							0

/* define controller bit for MEMCTL1_REG_21 */
#define MEMCTL1_REG_21_EMR_DATA_0						16

/* define controller bit for MEMCTL1_REG_22 */
#define MEMCTL1_REG_22_MR_DATA_1						16

/* define controller bit for MEMCTL1_REG_23 */
#define MEMCTL1_REG_23_EMR_DATA_1						16

/* define controller bit for MEMCTL1_REG_24 */
#define MEMCTL1_REG_24_COLUMN_SIZE						16
#define MEMCTL1_REG_24_ADDR_PINS						8

/* define controller bit for MEMCTL1_REG_27 */
#define MEMCTL1_REG_27_REDUC							16
#define MEMCTL1_REG_27_CS_MAP							8


/* define controller bit for MEMCTL1_REG_28 */
#define MEMCTL1_REG_28_RESYNC_DLL						16
#define MEMCTL1_REG_28_FAST_WRITE						16

/* define controller bit for MEMCTL1_REG_29 */
#define MEMCTL1_REG_29_INT_STATUS						8
#define MEMCTL1_REG_29_RESYNC_DLL_PER_AREF_EN			0

/* define controller bit for MEMCTL1_REG_30 */
#define MEMCTL1_REG_30_INT_MASK							16
#define MEMCTL1_REG_30_INT_ACK							0

/* define controller bit for MEMCTL1_REG_32 */
#define MEMCTL1_REG_32_OUT_OF_RANGE_SOURCE_ID			24
#define MEMCTL1_REG_32_OUT_OF_RANGE_TYPE				16
#define MEMCTL1_REG_32_OUT_OF_RANGE_LENGTH				8

/* define controller bit for MEMCTL1_REG_34 */
#define MEMCTL1_REG_34_PORT_DATA_ERROR_ID				24
#define MEMCTL1_REG_34_PORT_CMD_ERROR_TYPE				19
#define MEMCTL1_REG_34_PORT_CMD_ERROR_ID				8


/* define controller bit for MEMCTL1_REG_35 */
#define MEMCTL1_REG_35_W2R_DIFFCS_DLY					24
#define MEMCTL1_REG_35_R2W_DIFFCS_DLY					16
#define MEMCTL1_REG_35_R2R_DIFFCS_DLY					8

/* define controller bit for MEMCTL1_REG_36 */
#define MEMCTL1_REG_36_W2R_SAMECS_DLY					24
#define MEMCTL1_REG_36_R2W_SAMECS_DLY					16
#define MEMCTL1_REG_36_R2R_SAMECS_DLY					8
#define MEMCTL1_REG_36_W2W_DIFFCS_DLY					0

/* define controller bit for MEMCTL1_REG_38 */
#define MEMCTL1_REG_38_AXI_W_PRIORITY					24
#define MEMCTL1_REG_38_AXI_R_PRIORITY					16
#define MEMCTL1_REG_38_AXI_EN_SIZE_LT_WIDTH_INSTR		0

/* define controller bit for MEMCTL1_REG_39 */
#define MEMCTL1_REG_39_CKE_STATUS						8

/* define controller bit for MEMCTL1_REG_47 */
#define MEMCTL1_REG_47_MUST_BE_2						10

#define MEMCTL1_REG_47_CLK_PAD_DRIVE					8
#define MEMCTL1_REG_47_CLK_PAD_DRIVE_10MA				0
#define MEMCTL1_REG_47_CLK_PAD_DRIVE_8MA				1
#define MEMCTL1_REG_47_CLK_PAD_DRIVE_4MA				2
#define MEMCTL1_REG_47_CLK_PAD_DRIVE_2MA				3

#define MEMCTL1_REG_47_MUST_BE_1						2

#define MEMCTL1_REG_47_DRIVE							0
#define MEMCTL1_REG_47_DRIVE_10MA						0
#define MEMCTL1_REG_47_DRIVE_8MA						1
#define MEMCTL1_REG_47_DRIVE_4MA						2
#define MEMCTL1_REG_47_DRIVE_2MA						3
#endif
