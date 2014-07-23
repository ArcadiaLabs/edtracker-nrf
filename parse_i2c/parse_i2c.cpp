// parse_i2c.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BANK_SEL	0x6D
#define MEM_R_W		0x6F

void split_record(const std::string& in_str, std::vector<std::string>& out_vector, const char delim)
{
	out_vector.clear();

	std::string::const_iterator riter = in_str.begin();
	std::string field;
	while (riter != in_str.end())
	{
		if (*riter == delim)
		{
			out_vector.push_back(field);
			field.clear();
		} else {
			field += *riter;
		}

		++riter;
	}

	out_vector.push_back(field);
}

std::string get_reg_name(int reg)
{
	switch (reg)
	{
	case 0x19:	return "SMPLRT_DIV";
	case 0x1A:	return "CONFIG";
	case 0x1B:	return "GYRO_CONFIG";
	case 0x1C:	return "ACCEL_CONFIG";
	case 0x23:	return "FIFO_EN";
	case 0x37:	return "INT_PIN_CFG";
	case 0x38:	return "INT_ENABLE";
	case 0x3A:	return "INT_STATUS";
	case 0x3B:	return "ACCEL_XOUT_H";
	case 0x3C:	return "ACCEL_XOUT_L";
	case 0x3D:	return "ACCEL_YOUT_H";
	case 0x3E:	return "ACCEL_YOUT_L";
	case 0x3F:	return "ACCEL_ZOUT_H";
	case 0x40:	return "ACCEL_ZOUT_L";
	case 0x41:	return "TEMP_OUT_H";
	case 0x42:	return "TEMP_OUT_L";
	case 0x43:	return "GYOR_XOUT_H";
	case 0x44:	return "GYOR_XOUT_L";
	case 0x45:	return "GYOR_YOUT_H";
	case 0x46:	return "GYOR_YOUT_L";
	case 0x47:	return "GYOR_ZOUT_H";
	case 0x48:	return "GYOR_ZOUT_L";
	case 0x68:	return "SIGNAL_PATH_RESET";
	case 0x6A:	return "USER_CTRL";
	case 0x6B:	return "PWR_MGMT_1";
	case 0x6C:	return "PWR_MGMT_2";
	case 0x72:	return "FIFO_COUNT_H";
	case 0x73:	return "FIFO_COUNT_L";
	case 0x74:	return "FIFO_R_W";
	case 0x75:	return "WHO_AM_I";
	case 0x06:	return "ACCEL_OFFS";
	case 0x6D:	return "BANK_SEL";
	case 0x6E:	return "MEM_START_ADDR";
	case 0x6F:	return "MEM_R_W";
	case 0x70:	return "PRGM_START_H";
	case 0x71:	return "PRGM_START_L";
	}

	return "?";
}

std::string get_bank_name(uint16_t bank)
{
	switch (bank)
	{
	case (2712): return "CFG_LP_QUAT";
	case (1866): return "END_ORIENT_TEMP";
	case (2742): return "CFG_27";
	case (2224): return "CFG_20";
	case (2745): return "CFG_23";
	case (2690): return "CFG_FIFO_ON_EVENT";
	case (1761): return "END_PREDICTION_UPDATE";
	case (2620): return "CGNOTICE_INTR";
	case (1358): return "X_GRT_Y_TMP";
	case (1029): return "CFG_DR_INT";
	case (1035): return "CFG_AUTH";
	case (1835): return "UPDATE_PROP_ROT";
	case (1455): return "END_COMPARE_Y_X_TMP2";
	case (1359): return "SKIP_X_GRT_Y_TMP";
	case (1435): return "SKIP_END_COMPARE";
	case (1088): return "FCFG_3";
	case (1066): return "FCFG_2";
	case (1062): return "FCFG_1";
	case (1434): return "END_COMPARE_Y_X_TMP3";
	case (1073): return "FCFG_7";
	case (1106): return "FCFG_6";
	case (1713): return "FLAT_STATE_END";
	case (1616): return "SWING_END_4";
	case (1565): return "SWING_END_2";
	case (1587): return "SWING_END_3";
	case (1550): return "SWING_END_1";
	case (2718): return "CFG_8";
	case (2727): return "CFG_15";
	case (2746): return "CFG_16";
	case (1189): return "CFG_EXT_GYRO_BIAS";
	case (1407): return "END_COMPARE_Y_X_TMP";
	case (1839): return "DO_NOT_UPDATE_PROP_ROT";
	case (1205): return "CFG_7";
	case (1683): return "FLAT_STATE_END_TEMP";
	case (1484): return "END_COMPARE_Y_X";
	case (1551): return "SKIP_SWING_END_1";
	case (1588): return "SKIP_SWING_END_3";
	case (1566): return "SKIP_SWING_END_2";
	case (1672): return "TILTG75_START";
	case (2753): return "CFG_6";
	case (1669): return "TILTL75_END";
	case (1884): return "END_ORIENT";
	case (2573): return "CFG_FLICK_IN";
	case (1643): return "TILTL75_START";
	case (1208): return "CFG_MOTION_BIAS";
	case (1408): return "X_GRT_Y";
	case (2324): return "TEMPLABEL";
	case (1853): return "CFG_ANDROID_ORIENT_INT";
	case (2722): return "CFG_GYRO_RAW_DATA";
	case (1379): return "X_GRT_Y_TMP2";
	case (22+512): return "D_0_22";
	case (24+512): return "D_0_24";
	case (36): return "D_0_36";
	case (52): return "D_0_52";
	case (96): return "D_0_96";
	case (104): return "D_0_104";
	case (108): return "D_0_108";
	case (163): return "D_0_163";
	case (188): return "D_0_188";
	case (192): return "D_0_192";
	case (224): return "D_0_224";
	case (228): return "D_0_228";
	case (232): return "D_0_232";
	case (236): return "D_0_236";
	case (256 + 2): return "D_1_2";
	case (256 + 4): return "D_1_4";
	case (256 + 8): return "D_1_8";
	case (256 + 10): return "D_1_10";
	case (256 + 24): return "D_1_24";
	case (256 + 28): return "D_1_28";
	case (256 + 36): return "D_1_36";
	case (256 + 40): return "D_1_40";
	case (256 + 44): return "D_1_44";
	case (256 + 72): return "D_1_72";
	case (256 + 74): return "D_1_74";
	case (256 + 79): return "D_1_79";
	case (256 + 88): return "D_1_88";
	case (256 + 90): return "D_1_90";
	case (256 + 92): return "D_1_92";
	case (256 + 96): return "D_1_96";
	case (256 + 98): return "D_1_98";
	case (256 + 106): return "D_1_106";
	case (256 + 108): return "D_1_108";
	case (256 + 112): return "D_1_112";
	case (256 + 144): return "D_1_128";
	case (256 + 12): return "D_1_152";
	case (256 + 160): return "D_1_160";
	case (256 + 176): return "D_1_176";
	case (256 + 178): return "D_1_178";
	case (256 + 218): return "D_1_218";
	case (256 + 232): return "D_1_232";
	case (256 + 236): return "D_1_236";
	case (256 + 240): return "D_1_240";
	case (256 + 244): return "D_1_244";
	case (256 + 250): return "D_1_250";
	case (256 + 252): return "D_1_252";
	case (512 + 12): return "D_2_12";
	case (512 + 96): return "D_2_96";
	case (512 + 108): return "D_2_108";
	case (512 + 208): return "D_2_208";
	case (512 + 224): return "D_2_224";
	case (512 + 236): return "D_2_236";
	case (512 + 244): return "D_2_244";
	case (512 + 248): return "D_2_248";
	case (512 + 252): return "D_2_252";
	case 468:		return "DMP_TAP_THX";
	case 472:		return "DMP_TAP_THY";
	case 476:		return "DMP_TAP_THZ";
	case 478:		return "DMP_TAPW_MIN";
	}

	char buff[32];
	sprintf(buff, "0x%04x", bank);
	return buff;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::ifstream infile;
	infile.open("arduino_calib.txt");
	if (!infile.is_open())
	{
		puts("unable to open file");
		return -1;
	}

	char linebuff[256];
	infile.getline(linebuff, sizeof linebuff);
	int prev_packet_id = 0;
	uint16_t last_bank;
	std::vector<std::string> record;
	std::vector<uint8_t> packet;
	while (!infile.eof())
	{
		infile.getline(linebuff, sizeof linebuff);

		split_record(linebuff, record, ',');

		if (record.size() != 6)
			break;

		int packet_id = atoi(record[1].c_str());

		if (prev_packet_id != packet_id)
		{
			if (prev_packet_id == 27)
				puts("--- DMP firmware begin");

			bool is_write = packet.front() == 0xd0;

			std::vector<uint8_t>::iterator i = packet.begin() + 1;

			printf("// %i ", prev_packet_id);

			if (packet[1] == 0x6d)
				last_bank = (packet[2] << 8) | packet[3];

			if (is_write)
			{
				printf("W %s ", get_reg_name(packet[1]).c_str());
				
				if (packet[1] == 0x6D)	// is BANK_SEL?
				{
					printf("%s", get_bank_name(last_bank).c_str());
					i += 2;
				}
				
				++i;
			} else {
				printf("R ");
			}

			for (; i != packet.end(); ++i)
				printf("%02x ", *i);
			puts("");

			// make C source
			if (packet[1] == MEM_R_W  &&  is_write)
			{
				printf("\t{\n\tconst uint8_t __code arr[] = {");
				for (i = packet.begin() + 2; i != packet.end(); ++i)
					printf("%s0x%02x", (i == packet.begin() + 2 ? "" : ","), *i);
				puts("};");
				printf("\tmpu_write_mem(%s, sizeof arr, arr);\n", get_bank_name(last_bank).c_str());
				puts("\t}");
			}

			if (prev_packet_id == 987)
			{
				puts("--- DMP firmware end");
				puts("--- dmp_set_orientation()");
			} else if (prev_packet_id == 995) {
				puts("--- dmp_enable_feature()");
			}

			packet.clear();

			prev_packet_id = packet_id;
		}

		int addr = atoi(record[2].c_str());
		int data = atoi(record[3].c_str());

		if (packet.empty())
			packet.push_back(addr);

		packet.push_back(data);
	}

	return 0;
}
