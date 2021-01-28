
extern char LCD_str[32];
extern char LCD_input_str[32];
extern char LCD_input_str_[32];
extern char *LCD_crsr_position;

void LCD_init (void);
void LCD_reanim (void);
void LCD_all();
void LCD_put_str(char row, char column,const char *string);
