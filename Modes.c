

#include "Modes.h"
#include "RFDuino.h"


unsigned data0 = 0;
unsigned data1 = 0;
int counter = 0;
unsigned inPowerSaving = true;

int32_t ff[PARAMETRIC_MEASUREMENT_STORE_SIZE], humData[PARAMETRIC_MEASUREMENT_STORE_SIZE], tData[PARAMETRIC_MEASUREMENT_STORE_SIZE];
int32_t f0[PARAMETRIC_MEASUREMENT_STORE_SIZE],  f1[PARAMETRIC_MEASUREMENT_STORE_SIZE],  f2[PARAMETRIC_MEASUREMENT_STORE_SIZE],  f3[PARAMETRIC_MEASUREMENT_STORE_SIZE],  f4[PARAMETRIC_MEASUREMENT_STORE_SIZE];
void EnterPowerSaving(){
	SetGPIO(MCULED1_PORT, MCULED1_PIN, 0);
	SetGPIO(MCULED2_PORT, MCULED2_PIN, 0);
	SetGPIO(MCULED3_PORT, MCULED3_PIN, 0);
	EnterEM3();;
}

void PowerSavingModeNotification(unsigned mode){
	if(inPowerSaving && (mode != MODE0)){
		SendEmpty(100);
		for(int i=0; i<1; ++i){
			send_string("Leaving EM3 power saving.\n");
		}
		SendEmpty(100);
		inPowerSaving = false;
	}

	if(!inPowerSaving && mode == MODE0){
		SendEmpty(100);
		for(int i=0; i<1; ++i){
			send_string("Entering EM3 power saving.\n");
		}
		SendEmpty(100);
		inPowerSaving = true;
	}
}



void ContinousMeasurement(){
	SetGPIO(MCULED2_PORT, MCULED2_PIN, 1);
	InitADC();
			uint32_t f0 = GetADCvalue_Force0();
			double Volt = (ADC_to_Voltage(f0));
			double force = (Voltage_to_force(Volt));

			double hgmm = 133.3222365 * force;
			hgmm = hgmm / 38.0;

			InitRFDuino();
			//SendEmpty(n*5);

			SendEmpty(5);
			uint32_t humData;
			int32_t tData;
			SI7021_Measure(&humData, &tData);

			//send_string("Force[N]: \n");
			send_double(force);
			//send_string("F[Hgmm]: \n");
			send_double(hgmm);
			//send_string("T[°C]: \n");
			send_double(tData/1000.0);
			//send_string("H[%]: \n");
			send_double(humData/1000.0);
			//send_string("---------------------\n");
			//send_string("Voltage: \n");
			send_double(Volt);
			//send_string("---------------------\n");

			//send_string("Date\n:");
			//SendDate();
			send_string("---------------------\n");
			RFDuino_GiveIT();
			InitRFduinoUART();

//			send_int(ReadFromFlash((uint32_t*)ADDRESS_OF_LAST_DATA_ADDRESS));
			SendEmpty(5);
			SetGPIO(MCULED2_PORT, MCULED2_PIN, 0);
}


void ContinousMeasurement_for5fsr(){
	SetGPIO(MCULED1_PORT, MCULED1_PIN, 1);
	InitADC();
	uint32_t f0 = GetADCvalue_Force0();
	uint32_t f1 = GetADCvalue_Force1();
	uint32_t f2 = GetADCvalue_Force2();
	uint32_t f3 = GetADCvalue_Force3();
	uint32_t f4 = GetADCvalue_Force4();

	InitRFDuino();
	//SendEmpty(n*5);
	SendEmpty(5);
	uint32_t humData;
	int32_t tData;
	SI7021_Measure(&humData, &tData);
// ADC values of force sensors
	send_string("Force0");
	send_double(f0);
	send_string("Force1");
	send_double(f1);
	send_string("Force2");
	send_double(f2);
	send_string("Force3");
	send_double(f3);
	send_string("Force4");
	send_double(f4);
	send_string("---------------------\n");
	send_string("T[°C]: \n");
	send_double(tData/1000.0);
	send_string("H[%]: \n");
	send_double(humData/1000.0);
	send_string("---------------------\n");

	SendEmpty(5);
	SetGPIO(MCULED1_PORT, MCULED1_PIN, 0);
}

//CALIBRATION of FSR

double forceNewton(uint32_t f){

	double Volt = (ADC_to_Voltage(f));
	double force = (Voltage_to_force(Volt));//newton
	return force;

}

double forceing(uint32_t f){
	double temp = forceNewton(f);
	temp = temp * 101.972;
	return temp;
}
//finally the FSR measurement in Hgmm
double hgmm(uint32_t f){
	double temp = forceNewton(f);
	double hgmm = temp*91.81188;
	return hgmm;
}

void Measure_multipleFSR (int n, int period){

	EraseAllPages();
	SetGPIO(MCULED2_PORT, MCULED2_PIN, 1);
	uint32_t HumData;
	int32_t TData;
	uint32_t time_ms;

	uint32_t start_time = getTime();

		for(int i=0;i<n;++i){

			/*uint32_t f0 = GetADCvalue_Force0();
			uint32_t f1 = GetADCvalue_Force1();
			uint32_t f2 = GetADCvalue_Force2();
			uint32_t f3 = GetADCvalue_Force3();
			uint32_t f4 = GetADCvalue_Force4();*/
			f0[i] = GetADCvalue_Force0();
			f1[i] = GetADCvalue_Force1();
			f2[i] = GetADCvalue_Force2();
			f3[i] = GetADCvalue_Force3();
			f4[i] = GetADCvalue_Force4();

			SI7021_Measure(&HumData, &TData);


			/*uint32_t humData = HumData;
			uint32_t tData=TData;*/
			humData[i]=HumData;
			tData[i]=TData;

			time_ms = getTime()-start_time;
					//BatteryVoltage = BatteryADCMeasurement();

					/*WriteToFlash((uint32_t)f0);
					WriteToFlash((uint32_t)f1);
					WriteToFlash((uint32_t)f2);
					WriteToFlash((uint32_t)f3);
					WriteToFlash((uint32_t)f4);
					WriteToFlash((uint32_t)humData);
					WriteToFlash((uint32_t)tData);
					*/
					WriteToFlash((uint32_t)f0[i]);
					WriteToFlash((uint32_t)f1[i]);
					WriteToFlash((uint32_t)f2[i]);
					WriteToFlash((uint32_t)f3[i]);
					WriteToFlash((uint32_t)f4[i]);
					WriteToFlash((uint32_t)humData[i]);
					WriteToFlash((uint32_t)tData[i]);

					//WriteToFlash((uint32_t)time_ms);
					//WriteToFlash((uint32_t) BatteryVoltage);
					Delay(2);
					//Delay(100);
				}
		  //  WriteToFlash((uint32_t)time_ms);
			UpdateLastDataInFlash();

			RFDuino_GiveIT();
			InitRFduinoUART();
			SendEmpty(5);

			//send_string("------\n");

		//	for(uint32_t i=FLASH_START_ADDRESS; i<(FLASH_START_ADDRESS+(4*n*4)); i+=16){
			for(uint32_t i=FLASH_START_ADDRESS; i<(FLASH_START_ADDRESS+(7*n*4)); i+=28){
				uint32_t* address;
				uint32_t readValue;

				//force0
				address 	= (uint32_t*)(i);
				readValue 	= ReadFromFlash(address);
				//send_int(readValue);
				//send_double(quickMeas(readValue));
				send_double (hgmm(readValue));

				//force1
				address 	= (uint32_t*)(4+i);
				readValue 	= ReadFromFlash(address);
				send_double (hgmm(readValue));

				//force2
				address 	= (uint32_t*)(8+i);
				readValue 	= ReadFromFlash(address);
				send_double (hgmm(readValue));

				//force3
				address 	= (uint32_t*)(12+i);
				readValue 	= ReadFromFlash(address);
				send_double (hgmm(readValue));

				//force4
				address 	= (uint32_t*)(16+i);
				readValue 	= ReadFromFlash(address);
				send_double (hgmm(readValue));

				//humidity

				address 	= (uint32_t*)(20+i);
				readValue 	= ReadFromFlash(address);
				send_double((double)readValue/1000);


				//temperature

				address 	= (uint32_t*)(24+i);
				readValue 	= ReadFromFlash(address);
				send_double((double)readValue/1000);

		}

			/*for(int i=0; i<10; ++i){ //Measure battery after sending the n data
				send_int(BatteryADCMeasurement());
			}*/

			//send_string("Last data address:\n");
			//send_int(ReadFromFlash((uint32_t*)ADDRESS_OF_LAST_DATA_ADDRESS));
			//SendDate();
			//SendEmpty(5*n);
			SetGPIO(MCULED2_PORT, MCULED2_PIN, 0);
			//EnterPowerSaving();


}




