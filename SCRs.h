



void SCRsControl (void);
void CoolerFan_OnOff (void);

extern RUNPOWER rPwr; //runtime parameters for voltage and current regulation

void CurrentRegul (void);
void MainsVoltFollow (void);
void IspApproach (void);