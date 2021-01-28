
extern int Tclamps, Theatsink;

extern int ntcMF25_103_3435(int adcCode);

extern int mV0, mV1, mV2, rtdT0, rtdT1, TC2;
extern int Ht;//humidity;
extern int rhSp;// runtime setpoint for humidity regulator

int Calc_mV( int ADC, char Kg);
int Calc_Ohm( int mV);
int RTD_Pt100(int Ohm);
int Termocuple_K (int mV, int Tcool);

extern RUNPID runPid;

extern unsigned short Humid(unsigned short Tdry, unsigned short Twet);
void PID_regulator (int tC);
void putPIDoutput (void);
void circulationFAN (void);
void exhaustFAN (void);
void relHumidRegul (void);
int normSin (int percent, int range);