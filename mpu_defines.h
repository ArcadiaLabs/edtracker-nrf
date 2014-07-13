#ifndef MPU_DEFINES_H
#define MPU_DEFINES_H

/* These defines are copied from dmpDefaultMPU6050.c in the general MPL
 * releases. These defines may change for each DMP image, so be sure to modify
 * these values when switching to a new image.
 */
#define CFG_LP_QUAT             (2712)
#define END_ORIENT_TEMP         (1866)
#define CFG_27                  (2742)
#define CFG_20                  (2224)
#define CFG_23                  (2745)
#define CFG_FIFO_ON_EVENT       (2690)
#define END_PREDICTION_UPDATE   (1761)
#define CGNOTICE_INTR           (2620)
#define X_GRT_Y_TMP             (1358)
#define CFG_DR_INT              (1029)
#define CFG_AUTH                (1035)
#define UPDATE_PROP_ROT         (1835)
#define END_COMPARE_Y_X_TMP2    (1455)
#define SKIP_X_GRT_Y_TMP        (1359)
#define SKIP_END_COMPARE        (1435)
#define FCFG_3                  (1088)
#define FCFG_2                  (1066)
#define FCFG_1                  (1062)
#define END_COMPARE_Y_X_TMP3    (1434)
#define FCFG_7                  (1073)
#define FCFG_6                  (1106)
#define FLAT_STATE_END          (1713)
#define SWING_END_4             (1616)
#define SWING_END_2             (1565)
#define SWING_END_3             (1587)
#define SWING_END_1             (1550)
#define CFG_8                   (2718)
#define CFG_15                  (2727)
#define CFG_16                  (2746)
#define CFG_EXT_GYRO_BIAS       (1189)
#define END_COMPARE_Y_X_TMP     (1407)
#define DO_NOT_UPDATE_PROP_ROT  (1839)
#define CFG_7                   (1205)
#define FLAT_STATE_END_TEMP     (1683)
#define END_COMPARE_Y_X         (1484)
#define SKIP_SWING_END_1        (1551)
#define SKIP_SWING_END_3        (1588)
#define SKIP_SWING_END_2        (1566)
#define TILTG75_START           (1672)
#define CFG_6                   (2753)
#define TILTL75_END             (1669)
#define END_ORIENT              (1884)
#define CFG_FLICK_IN            (2573)
#define TILTL75_START           (1643)
#define CFG_MOTION_BIAS         (1208)
#define X_GRT_Y                 (1408)
#define TEMPLABEL               (2324)
#define CFG_ANDROID_ORIENT_INT  (1853)
#define CFG_GYRO_RAW_DATA       (2722)
#define X_GRT_Y_TMP2            (1379)

#define D_0_22                  (22+512)
#define D_0_24                  (24+512)

#define D_0_36                  (36)
#define D_0_52                  (52)
#define D_0_96                  (96)
#define D_0_104                 (104)
#define D_0_108                 (108)
#define D_0_163                 (163)
#define D_0_188                 (188)
#define D_0_192                 (192)
#define D_0_224                 (224)
#define D_0_228                 (228)
#define D_0_232                 (232)
#define D_0_236                 (236)

#define D_1_2                   (256 + 2)
#define D_1_4                   (256 + 4)
#define D_1_8                   (256 + 8)
#define D_1_10                  (256 + 10)
#define D_1_24                  (256 + 24)
#define D_1_28                  (256 + 28)
#define D_1_36                  (256 + 36)
#define D_1_40                  (256 + 40)
#define D_1_44                  (256 + 44)
#define D_1_72                  (256 + 72)
#define D_1_74                  (256 + 74)
#define D_1_79                  (256 + 79)
#define D_1_88                  (256 + 88)
#define D_1_90                  (256 + 90)
#define D_1_92                  (256 + 92)
#define D_1_96                  (256 + 96)
#define D_1_98                  (256 + 98)
#define D_1_106                 (256 + 106)
#define D_1_108                 (256 + 108)
#define D_1_112                 (256 + 112)
#define D_1_128                 (256 + 144)
#define D_1_152                 (256 + 12)
#define D_1_160                 (256 + 160)
#define D_1_176                 (256 + 176)
#define D_1_178                 (256 + 178)
#define D_1_218                 (256 + 218)
#define D_1_232                 (256 + 232)
#define D_1_236                 (256 + 236)
#define D_1_240                 (256 + 240)
#define D_1_244                 (256 + 244)
#define D_1_250                 (256 + 250)
#define D_1_252                 (256 + 252)
#define D_2_12                  (512 + 12)
#define D_2_96                  (512 + 96)
#define D_2_108                 (512 + 108)
#define D_2_208                 (512 + 208)
#define D_2_224                 (512 + 224)
#define D_2_236                 (512 + 236)
#define D_2_244                 (512 + 244)
#define D_2_248                 (512 + 248)
#define D_2_252                 (512 + 252)

#define CPASS_BIAS_X            (35 * 16 + 4)
#define CPASS_BIAS_Y            (35 * 16 + 8)
#define CPASS_BIAS_Z            (35 * 16 + 12)
#define CPASS_MTX_00            (36 * 16)
#define CPASS_MTX_01            (36 * 16 + 4)
#define CPASS_MTX_02            (36 * 16 + 8)
#define CPASS_MTX_10            (36 * 16 + 12)
#define CPASS_MTX_11            (37 * 16)
#define CPASS_MTX_12            (37 * 16 + 4)
#define CPASS_MTX_20            (37 * 16 + 8)
#define CPASS_MTX_21            (37 * 16 + 12)
#define CPASS_MTX_22            (43 * 16 + 12)
#define D_EXT_GYRO_BIAS_X       (61 * 16)
#define D_EXT_GYRO_BIAS_Y       (61 * 16) + 4
#define D_EXT_GYRO_BIAS_Z       (61 * 16) + 8
#define D_ACT0                  (40 * 16)
#define D_ACSX                  (40 * 16 + 4)
#define D_ACSY                  (40 * 16 + 8)
#define D_ACSZ                  (40 * 16 + 12)

#define FLICK_MSG               (45 * 16 + 4)
#define FLICK_COUNTER           (45 * 16 + 8)
#define FLICK_LOWER             (45 * 16 + 12)
#define FLICK_UPPER             (46 * 16 + 12)

#define D_AUTH_OUT              (992)
#define D_AUTH_IN               (996)
#define D_AUTH_A                (1000)
#define D_AUTH_B                (1004)

#define D_PEDSTD_BP_B           (768 + 0x1C)
#define D_PEDSTD_HP_A           (768 + 0x78)
#define D_PEDSTD_HP_B           (768 + 0x7C)
#define D_PEDSTD_BP_A4          (768 + 0x40)
#define D_PEDSTD_BP_A3          (768 + 0x44)
#define D_PEDSTD_BP_A2          (768 + 0x48)
#define D_PEDSTD_BP_A1          (768 + 0x4C)
#define D_PEDSTD_INT_THRSH      (768 + 0x68)
#define D_PEDSTD_CLIP           (768 + 0x6C)
#define D_PEDSTD_SB             (768 + 0x28)
#define D_PEDSTD_SB_TIME        (768 + 0x2C)
#define D_PEDSTD_PEAKTHRSH      (768 + 0x98)
#define D_PEDSTD_TIML           (768 + 0x2A)
#define D_PEDSTD_TIMH           (768 + 0x2E)
#define D_PEDSTD_PEAK           (768 + 0X94)
#define D_PEDSTD_STEPCTR        (768 + 0x60)
#define D_PEDSTD_TIMECTR        (964)
#define D_PEDSTD_DECI           (768 + 0xA0)

#define D_HOST_NO_MOT           (976)
#define D_ACCEL_BIAS            (660)

#define D_ORIENT_GAP            (76)

#define D_TILT0_H               (48)
#define D_TILT0_L               (50)
#define D_TILT1_H               (52)
#define D_TILT1_L               (54)
#define D_TILT2_H               (56)
#define D_TILT2_L               (58)
#define D_TILT3_H               (60)
#define D_TILT3_L               (62)

#define DMP_CODE_SIZE           3062

// dmpmap.h

#define DMP_PTAT    0
#define DMP_XGYR    2
#define DMP_YGYR    4
#define DMP_ZGYR    6
#define DMP_XACC    8
#define DMP_YACC    10
#define DMP_ZACC    12
#define DMP_ADC1    14
#define DMP_ADC2    16
#define DMP_ADC3    18
#define DMP_BIASUNC    20
#define DMP_FIFORT    22
#define DMP_INVGSFH    24
#define DMP_INVGSFL    26
#define DMP_1H    28
#define DMP_1L    30
#define DMP_BLPFSTCH    32
#define DMP_BLPFSTCL    34
#define DMP_BLPFSXH    36
#define DMP_BLPFSXL    38
#define DMP_BLPFSYH    40
#define DMP_BLPFSYL    42
#define DMP_BLPFSZH    44
#define DMP_BLPFSZL    46
#define DMP_BLPFMTC    48
#define DMP_SMC    50
#define DMP_BLPFMXH    52
#define DMP_BLPFMXL    54
#define DMP_BLPFMYH    56
#define DMP_BLPFMYL    58
#define DMP_BLPFMZH    60
#define DMP_BLPFMZL    62
#define DMP_BLPFC    64
#define DMP_SMCTH    66
#define DMP_0H2    68
#define DMP_0L2    70
#define DMP_BERR2H    72
#define DMP_BERR2L    74
#define DMP_BERR2NH    76
#define DMP_SMCINC    78
#define DMP_ANGVBXH    80
#define DMP_ANGVBXL    82
#define DMP_ANGVBYH    84
#define DMP_ANGVBYL    86
#define DMP_ANGVBZH    88
#define DMP_ANGVBZL    90
#define DMP_BERR1H    92
#define DMP_BERR1L    94
#define DMP_ATCH    96
#define DMP_BIASUNCSF    98
#define DMP_ACT2H    100
#define DMP_ACT2L    102
#define DMP_GSFH    104
#define DMP_GSFL    106
#define DMP_GH    108
#define DMP_GL    110
#define DMP_0_5H    112
#define DMP_0_5L    114
#define DMP_0_0H    116
#define DMP_0_0L    118
#define DMP_1_0H    120
#define DMP_1_0L    122
#define DMP_1_5H    124
#define DMP_1_5L    126
#define DMP_TMP1AH    128
#define DMP_TMP1AL    130
#define DMP_TMP2AH    132
#define DMP_TMP2AL    134
#define DMP_TMP3AH    136
#define DMP_TMP3AL    138
#define DMP_TMP4AH    140
#define DMP_TMP4AL    142
#define DMP_XACCW    144
#define DMP_TMP5    146
#define DMP_XACCB    148
#define DMP_TMP8    150
#define DMP_YACCB    152
#define DMP_TMP9    154
#define DMP_ZACCB    156
#define DMP_TMP10    158
#define DMP_DZH    160
#define DMP_DZL    162
#define DMP_XGCH    164
#define DMP_XGCL    166
#define DMP_YGCH    168
#define DMP_YGCL    170
#define DMP_ZGCH    172
#define DMP_ZGCL    174
#define DMP_YACCW    176
#define DMP_TMP7    178
#define DMP_AFB1H    180
#define DMP_AFB1L    182
#define DMP_AFB2H    184
#define DMP_AFB2L    186
#define DMP_MAGFBH    188
#define DMP_MAGFBL    190
#define DMP_QT1H    192
#define DMP_QT1L    194
#define DMP_QT2H    196
#define DMP_QT2L    198
#define DMP_QT3H    200
#define DMP_QT3L    202
#define DMP_QT4H    204
#define DMP_QT4L    206
#define DMP_CTRL1H    208
#define DMP_CTRL1L    210
#define DMP_CTRL2H    212
#define DMP_CTRL2L    214
#define DMP_CTRL3H    216
#define DMP_CTRL3L    218
#define DMP_CTRL4H    220
#define DMP_CTRL4L    222
#define DMP_CTRLS1    224
#define DMP_CTRLSF1    226
#define DMP_CTRLS2    228
#define DMP_CTRLSF2    230
#define DMP_CTRLS3    232
#define DMP_CTRLSFNLL    234
#define DMP_CTRLS4    236
#define DMP_CTRLSFNL2    238
#define DMP_CTRLSFNL    240
#define DMP_TMP30    242
#define DMP_CTRLSFJT    244
#define DMP_TMP31    246
#define DMP_TMP11    248
#define DMP_CTRLSF2_2    250
#define DMP_TMP12    252
#define DMP_CTRLSF1_2    254
#define DMP_PREVPTAT    256
#define DMP_ACCZB    258
#define DMP_ACCXB    264
#define DMP_ACCYB    266
#define DMP_1HB    272
#define DMP_1LB    274
#define DMP_0H    276
#define DMP_0L    278
#define DMP_ASR22H    280
#define DMP_ASR22L    282
#define DMP_ASR6H    284
#define DMP_ASR6L    286
#define DMP_TMP13    288
#define DMP_TMP14    290
#define DMP_FINTXH    292
#define DMP_FINTXL    294
#define DMP_FINTYH    296
#define DMP_FINTYL    298
#define DMP_FINTZH    300
#define DMP_FINTZL    302
#define DMP_TMP1BH    304
#define DMP_TMP1BL    306
#define DMP_TMP2BH    308
#define DMP_TMP2BL    310
#define DMP_TMP3BH    312
#define DMP_TMP3BL    314
#define DMP_TMP4BH    316
#define DMP_TMP4BL    318
#define DMP_STXG    320
#define DMP_ZCTXG    322
#define DMP_STYG    324
#define DMP_ZCTYG    326
#define DMP_STZG    328
#define DMP_ZCTZG    330
#define DMP_CTRLSFJT2    332
#define DMP_CTRLSFJTCNT    334
#define DMP_PVXG    336
#define DMP_TMP15    338
#define DMP_PVYG    340
#define DMP_TMP16    342
#define DMP_PVZG    344
#define DMP_TMP17    346
#define DMP_MNMFLAGH    352
#define DMP_MNMFLAGL    354
#define DMP_MNMTMH    356
#define DMP_MNMTML    358
#define DMP_MNMTMTHRH    360
#define DMP_MNMTMTHRL    362
#define DMP_MNMTHRH    364
#define DMP_MNMTHRL    366
#define DMP_ACCQD4H    368
#define DMP_ACCQD4L    370
#define DMP_ACCQD5H    372
#define DMP_ACCQD5L    374
#define DMP_ACCQD6H    376
#define DMP_ACCQD6L    378
#define DMP_ACCQD7H    380
#define DMP_ACCQD7L    382
#define DMP_ACCQD0H    384
#define DMP_ACCQD0L    386
#define DMP_ACCQD1H    388
#define DMP_ACCQD1L    390
#define DMP_ACCQD2H    392
#define DMP_ACCQD2L    394
#define DMP_ACCQD3H    396
#define DMP_ACCQD3L    398
#define DMP_XN2H    400
#define DMP_XN2L    402
#define DMP_XN1H    404
#define DMP_XN1L    406
#define DMP_YN2H    408
#define DMP_YN2L    410
#define DMP_YN1H    412
#define DMP_YN1L    414
#define DMP_YH    416
#define DMP_YL    418
#define DMP_B0H    420
#define DMP_B0L    422
#define DMP_A1H    424
#define DMP_A1L    426
#define DMP_A2H    428
#define DMP_A2L    430
#define DMP_SEM1    432
#define DMP_FIFOCNT    434
#define DMP_SH_TH_X    436
#define DMP_PACKET    438
#define DMP_SH_TH_Y    440
#define DMP_FOOTER    442
#define DMP_SH_TH_Z    444
#define DMP_TEMP29    448
#define DMP_TEMP30    450
#define DMP_XACCB_PRE    452
#define DMP_XACCB_PREL    454
#define DMP_YACCB_PRE    456
#define DMP_YACCB_PREL    458
#define DMP_ZACCB_PRE    460
#define DMP_ZACCB_PREL    462
#define DMP_TMP22    464
#define DMP_TAP_TIMER    466
#define DMP_TAP_THX    468
#define DMP_TAP_THY    472
#define DMP_TAP_THZ    476
#define DMP_TAPW_MIN    478
#define DMP_TMP25    480
#define DMP_TMP26    482
#define DMP_TMP27    484
#define DMP_TMP28    486
#define DMP_ORIENT    488
#define DMP_THRSH    490
#define DMP_ENDIANH    492
#define DMP_ENDIANL    494
#define DMP_BLPFNMTCH    496
#define DMP_BLPFNMTCL    498
#define DMP_BLPFNMXH    500
#define DMP_BLPFNMXL    502
#define DMP_BLPFNMYH    504
#define DMP_BLPFNMYL    506
#define DMP_BLPFNMZH    508
#define DMP_BLPFNMZL    510


#define BIT_I2C_MST_VDDIO   (0x80)
#define BIT_FIFO_EN         (0x40)
#define BIT_DMP_EN          (0x80)
#define BIT_FIFO_RST        (0x04)
#define BIT_DMP_RST         (0x08)
#define BIT_FIFO_OVERFLOW   (0x10)
#define BIT_DATA_RDY_EN     (0x01)
#define BIT_DMP_INT_EN      (0x02)
#define BIT_MOT_INT_EN      (0x40)
#define BITS_FSR            (0x18)
#define BITS_LPF            (0x07)
#define BITS_HPF            (0x07)
#define BITS_CLK            (0x07)
#define BIT_FIFO_SIZE_1024  (0x40)
#define BIT_FIFO_SIZE_2048  (0x80)
#define BIT_FIFO_SIZE_4096  (0xC0)
#define BIT_RESET           (0x80)
#define BIT_SLEEP           (0x40)
#define BIT_S0_DELAY_EN     (0x01)
#define BIT_S2_DELAY_EN     (0x04)
#define BITS_SLAVE_LENGTH   (0x0F)
#define BIT_SLAVE_BYTE_SW   (0x40)
#define BIT_SLAVE_GROUP     (0x10)
#define BIT_SLAVE_EN        (0x80)
#define BIT_I2C_READ        (0x80)
#define BITS_I2C_MASTER_DLY (0x1F)
#define BIT_AUX_IF_EN       (0x20)
#define BIT_ACTL            (0x80)
#define BIT_LATCH_EN        (0x20)
#define BIT_ANY_RD_CLR      (0x10)
#define BIT_BYPASS_EN       (0x02)
#define BITS_WOM_EN         (0xC0)
#define BIT_LPA_CYCLE       (0x20)
#define BIT_STBY_XA         (0x20)
#define BIT_STBY_YA         (0x10)
#define BIT_STBY_ZA         (0x08)
#define BIT_STBY_XG         (0x04)
#define BIT_STBY_YG         (0x02)
#define BIT_STBY_ZG         (0x01)
#define BIT_STBY_XYZA       (BIT_STBY_XA | BIT_STBY_YA | BIT_STBY_ZA)
#define BIT_STBY_XYZG       (BIT_STBY_XG | BIT_STBY_YG | BIT_STBY_ZG)

#define INV_FILTER_256HZ_NOLPF2		0
#define INV_FILTER_188HZ			1
#define INV_FILTER_98HZ				2
#define INV_FILTER_42HZ				3
#define INV_FILTER_20HZ				4
#define INV_FILTER_10HZ				5
#define INV_FILTER_5HZ				6
#define INV_FILTER_2100HZ_NOLPF		7
#define NUM_FILTER					8

#endif	// MPU_DEFINES_H