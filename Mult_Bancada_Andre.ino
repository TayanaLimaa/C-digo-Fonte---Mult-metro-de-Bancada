//Código para o Multimetro Digital de Bancada
// Trabalho de Conclusão de Curso
// Faculdade de Engenharia Elétrica
// Campus Universitário de Tucuruí
// Universidade Federal do Pará
//Discente: Tayana Lima
// Orientadores: Prof. Dr. André Felipe Souza da Cruz
              // Prof. Dr. Rafael Suzuki Bayma
//Declaração das bibliotecas:
#include <Wire.h> // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h> // Biblioteca utilizada para fazer a comunicação com o display 20x4 
LiquidCrystal_I2C lcd(0x27,16,2); //Define o tipo do display LCD


//Declaração das variáveis Globais:
//----------------------------------------------------------
// Declarando os botões de comando:
#define menu 13 // Define o botão de Menu
#define enter 8 // Define o botão de Enter
// Definindo as funções para operar os botões de comando:
void readButts();  // Função para ler os botões
void list_menu(); // Função para listar o menu
void menu_select(); // Função para selecionar o menu
//Declarando as variáveis globais da Lista do MENU (Vetores)
int      line[4] = {0,1,2,3}, //Posições das linhas das funções, de 0 a 3 (4 funções)
         line_bk[4],    // Vetor de backup, necessário para rotação das funções
         index, menu_number = 1; //Variaveis usadas no menu de seleção
boolean  menu_flag  = 0, enter_flag = 0,sub_menu = 0; // Flags usadas no menu de seleção
// Declarando os valores das constantes que definem os sinais de medição de tensão:
 float V_max_REF[3] = {5.05,5.02,4.85}; //Tensão de referência para a medição do voltimetro
 float Escala_tensao[3] = {1,4,40}; //Possiveis escalas 1, 4, 40                          !!!! Definir a partir da chave seletora
 int Escala_tensao_inteira[3] = {5,20,200}; 
// Declarando os valores das constantes que definem os sinais de medição de corrente:      
float voltage_A2;
float Sensibilidadcc = 0.068;  // Modelo 30A [A/A] 0.063
int NumAmostras = 100;  // Declaração do número de Amostras do conversorADC                       
//int Escala_corrente_inteira = 30;  //                                                  !!!! Definir a partir da chave seletora
// Declarando os valores das constantes que definem os sinais de medição de resistência elétrica:   
float voltage_A3;
float V_in = 5;
float RA = 0;
float RB = 0;
float buffer = 0;
//CONFIGURAÇÃO das portas digitais conectada aos RESISTORES
int led_testeCC = 0;
int Apply_voltage = 2;
int ch_2K = 3;
int ch_20K = 4;
int ch_200K = 5;
int ch_1M = 6;
//CONFIGURAÇÃO DA CHAVE DE SELEÇÃO
int Escala_2k =9;
int Escala_20k =10;
int Escala_200k =11;
int Escala_1M =12;
int Escala_continuidade = 7;
//----------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  // Teclas de comando:
   pinMode(menu,  INPUT_PULLUP);
   pinMode(enter, INPUT_PULLUP);
   // Pinos do ohmimetro:
  pinMode(led_testeCC,OUTPUT);
  pinMode(Apply_voltage, OUTPUT);
  pinMode(Escala_2k, INPUT);
  pinMode(Escala_20k, INPUT);
  pinMode(Escala_200k, INPUT);
  pinMode(Escala_1M, INPUT);
  pinMode(Escala_continuidade, INPUT);
  pinMode(ch_2K, INPUT);
  pinMode(ch_20K, INPUT);
  pinMode(ch_200K, INPUT);
  pinMode(ch_1M, INPUT);
  Serial.begin(9600);
//----------------------------------------------------------
// Iniciando o funcionamento do display:
  lcd.init(); // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight(); // Serve para ligar a luz do display
  lcd.begin (16,3); // Inicia o display em colunas e linhas
  lcd.clear(); // Serve para limpar a tela do display
  //Texto de apresentação:
  lcd.setCursor(0,0); lcd.print("Bem vindo!         FEE-CAMTUC-UFPA");
  lcd.setCursor(0,1); lcd.print("Multimetro Digital");
  delay(2500);
for(int posi_LCD = 0; posi_LCD < 18; posi_LCD ++)
 {lcd.setCursor(0,0);lcd.scrollDisplayLeft();delay(100);}// delay: 500
  delay(750);
  lcd.clear(); // Serve para limpar a tela do display
  lcd.setCursor(0,0); lcd.print(">"); // Seta o cursor > na posição 0,0
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
// Função definida para mostrar no serial monitor os valores medidos pelas portas analógicas do Arduino
void MostrarMedicao(String magnitud, float valor, String unidad){
  Serial.print(magnitud);  Serial.print(valor, 3);  Serial.println(unidad);
  delay(1000);                                                   }

//-----------------------------------------------------------------------------------------------------------------------------------------------
void loop() { 
// Indicação do Menu no display LCD:
digitalWrite(led_testeCC, LOW);
lcd.setCursor(0,0); lcd.print(">"); // Seta o cursor > na posição 0,0
 if(!sub_menu)
  { lcd.setCursor(1,line[0]);
    lcd.print("A) Volt CC       ");
    lcd.setCursor(1,line[1]);
    lcd.print("B) Amp CC   ");
    lcd.setCursor(1,line[2]);
    lcd.print("C) Ohmimetro");
    lcd.setCursor(1,line[3]);
    lcd.print("D) Watimetro");
     }
    readButts();
             } 
//-----------------------------------------------------------------------------------------------------------------------------------------------
// Leitura dos botões para executar seleção do medidor no menu:
  void readButts() { 
  int B = sizeof(line)/sizeof(line[0]); // Variavel com a dimensão do array line
   if(!digitalRead(menu))  menu_flag  = 0x01; // Lê o estado lógico da entrada 13, menu, e salva na flag
   if(digitalRead(menu) && menu_flag) { // Se o botão é precionado, ao mesmo tempo que a flag é 1, então:
      menu_flag = 0x00; //zera a flag
      list_menu(); // Chama a função lista, para descer as opções no menu
      menu_number+=1; // Variavel de adição sempre que o botão menu for pressionado
      if(menu_number > B) menu_number = 1; // Se a variavel ultrapassar 4, volte para 1
     }

if(!digitalRead(enter)) enter_flag = 0x01; // Lê o estado lógico da entrada 8, enter, e salva na flag
   if(digitalRead(enter) && enter_flag) { // Se o botão é precionado, ao mesmo tempo que a flag é 1, então:
      enter_flag = 0x00; //zera a flag
      //sub_menu = !sub_menu; // inverte o estado lógico da variável sub_menu
      menu_select();
       } 
}  
//-----------------------------------------------------------------------------------------------------------------------------------------------
// Seleção do medidor no menu a partir dos botões lidos
void list_menu() {
  int A = sizeof(line)/sizeof(line[0])-1; // Variavel com a dimensão do array line menos um
  for(int i=A; i>-1; i--) { // A congatem inicia com i=2, diminuindo de 1 em 1, enquanto i maior que -1;
    index = i-1; //Indice igual a i menos 1
    line_bk[i] = line[i]; //Salva o vetor line atual no line_backup
    if(index < 0) line[i] = line_bk[i+A]; //Se o indice for <0, resetar o line para o line[inicial]
    else line[i] = line[i-1]; // Desloca as colunas do vetor line, para fazer as funções subirem no display LCD
                           }
                 }
//-----------------------------------------------------------------------------------------------------------------------------------------------
// Ao selecionar uma das opções de medidor, executar a devida função de medição:
void menu_select() {
    switch(menu_number)  {
//- Voltimetro CC: ---------------------------------------------------------
       case 1: 
            do{
            int ii=0;
            int sensorValue = analogRead(A0); // Leitura da porta analógica do arduino A0
            int escalaValue =  analogRead(A1); // Leitura da porta analógica do arduino A1
            if (escalaValue>400 && escalaValue<800 ) ii=1;
            if (escalaValue>0 && escalaValue<400 ) ii=2;
            //Serial.println(escalaValue);  Serial.println(ii);
            float voltage_A0 = sensorValue *Escala_tensao[ii]* (V_max_REF[ii] / 1023.0);    // Declaração da variável Tensão
            // Imprimir no display LCD as informações de tensão
            if (voltage_A0<Escala_tensao_inteira[ii]){
            lcd.clear();
            lcd.setCursor(0, 0);lcd.print("ESCALA");
            lcd.setCursor(7, 0);lcd.print(Escala_tensao_inteira[ii]);
            lcd.setCursor(10, 0);lcd.print("V");
            lcd.setCursor(0, 1);lcd.print("Tensao:");
            lcd.setCursor(9, 1);lcd.print(voltage_A0);
            lcd.setCursor(15, 1);lcd.print("V");
            delay(500);}
            else{
             lcd.clear();
            lcd.setCursor(0, 0);lcd.print("ESCALA");
            lcd.setCursor(7, 0);lcd.print(Escala_tensao_inteira[ii]);
            lcd.setCursor(10, 0);lcd.print("V");
            lcd.setCursor(0, 1);lcd.print("Mudar de Escala:");
            delay(1500);
            }
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");
            delay(10000);
            }
            } while (digitalRead(menu));
             lcd.clear(); // Serve para limpar a tela do display
             lcd.setCursor(1,0); lcd.print("Voltar ao Menu");
             delay(2000);
             lcd.clear(); // Serve para limpar a tela do display
            break;
//--------------------------------------------------------------------------
//- Amperimetro CC: --------------------------------------------------------
       case 2: 
            do{
              float corrente_SUM = 0;
              for (int i = 0; i < NumAmostras; i++) {    // 100 amostras de leitura analógica do sensor ACS712 para maior precisão
                voltage_A2 = analogRead(A2) * 5.0 / 1023.0;
                corrente_SUM += (voltage_A2 - 2.4955) / Sensibilidadcc;// Valor da sensibilidade é de 66mV/A
                                                    }  
              float corrente = (corrente_SUM / NumAmostras);
              // Mostrar no serial monitor a corrente média calculada e o sinal de tensão na porta A1:
              //MostrarMedicao("Intensidad: ", corrente, " A ,");  MostrarMedicao("SaidadoSensor: ", voltage_A1, " V"); 
            // Imprimir no display LCD as informações de corrente:
            if ( (corrente > -0.100) && (corrente < 0.100))  {
                lcd.clear();lcd.setCursor(2, 0); lcd.print("Sem leitura"); // Imprimir no LCD: Sem Leitura
                lcd.setCursor(1, 1); lcd.print("Corrente 0A"); // Imprimir no LCD: Corrente 0A
                delay(500); }
             else {
            lcd.clear();
            lcd.setCursor(0, 0);lcd.print("Faixa");
            lcd.setCursor(6, 0);lcd.print("-30A a 30A");
            lcd.setCursor(0, 1);lcd.print("Corrente:");
            lcd.setCursor(9, 1);lcd.print(corrente);
            lcd.setCursor(15, 1);lcd.print("A");
            delay(500);
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");
            delay(10000);
                                         }
                  }
             } while (digitalRead(menu));
             lcd.clear(); // Serve para limpar a tela do display
             lcd.setCursor(1,0); lcd.print("Voltar ao Menu");
             delay(2000);
             lcd.clear(); // Serve para limpar a tela do display
             break;
//--------------------------------------------------------------------------
//- Ohmimetro CC: --------------------------------------------------------           
       case 3: 
            do{ 
              digitalWrite(led_testeCC, LOW);// Apagar o LED de teste de continuidade
              //Escala de 2Kohm: -----     ---------       -----------      ---------        -------       ---------- 
              if (digitalRead(Escala_2k)) {
              // Níveis elétricos das saídas digitais do arduino, para executar a leitura de resitência na escala 2Kohm 
              pinMode(ch_2K,OUTPUT); pinMode(ch_20K,INPUT);  pinMode(ch_200K,INPUT);  pinMode(ch_1M,INPUT);
              digitalWrite(Apply_voltage, HIGH); digitalWrite(ch_2K, LOW);
              // Cálculo da resistência a partir da leitura da entrada A3:
              float R1 = 2;
              voltage_A3 = analogRead(A3);
              buffer = voltage_A3 * V_in;
              float V_out = (buffer) / 1024.0;                          
              buffer = (V_in / V_out) - 1;
              float R2 = R1 * buffer*1000; //1000 porque o expressamos em ohms
            // Imprimir a informação de leitura de resistência
              if (R2 <= 2000) {
                lcd.clear();// Limpar o display
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 2K");// Informação da escala
                lcd.setCursor(0,1);  lcd.print("Res= "); //Resistência lida
                lcd.setCursor(5,1);  lcd.print(R2); //Resistência lida
                lcd.setCursor(11,1); lcd.print(" ohms");// Unidade
                delay(500);
                              } else {
                lcd.clear();
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 2K"); // Informação da escala
                lcd.setCursor(0,1);  lcd.print("Sem leitura de R");// Resistência maior que a escala
                delay(500);    
                                     }
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");// Informa a função hold
            delay(10000);               }
            }
            
            //Escala de 20Kohm: -----     ---------       -----------      ---------        -------       ---------- 
            if (digitalRead(Escala_20k)) {
              // Níveis elétricos das saídas digitais do arduino, para executar a leitura de resitência na escala 20Kohm 
              pinMode(ch_2K,INPUT); pinMode(ch_20K,OUTPUT);  pinMode(ch_200K,INPUT);  pinMode(ch_1M,INPUT);
              digitalWrite(Apply_voltage, HIGH); digitalWrite(ch_20K, LOW);
              // Cálculo da resistência a partir da leitura da entrada A3:
              float R1 = 20;
              voltage_A3 = analogRead(A3);
              buffer = voltage_A3 * V_in;
              float V_out = (buffer) / 1024.0;                          
              buffer = (V_in / V_out) - 1;
              float R2 = R1 * buffer;
            // Imprimir a informação de leitura de resistência
              if (R2 <= 20) {
                lcd.clear();// Limpar o display
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 20K");// Informação da escala
                lcd.setCursor(0,1);  lcd.print("Res= "); //Resistência lida
                lcd.setCursor(5,1);  lcd.print(R2); //Resistência lida
                lcd.setCursor(11,1); lcd.print("Kohms");// Unidade
                delay(500);
                              } else {
                lcd.clear();
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 20K"); // Informação da escala
                lcd.setCursor(0,1);  lcd.print("Sem leitura de R");// Resistência maior que a escala
                delay(500);    
                                     }
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");// Informa a função hold
            delay(10000);               }
            }

            //Escala de 200Kohm: -----     ---------       -----------      ---------        -------       ---------- 
            if (digitalRead(Escala_200k)) {
              // Níveis elétricos das saídas digitais do arduino, para executar a leitura de resitência na escala 200Kohm 
              pinMode(ch_2K,INPUT); pinMode(ch_20K,INPUT);  pinMode(ch_200K,OUTPUT);  pinMode(ch_1M,INPUT);
              digitalWrite(Apply_voltage, HIGH); digitalWrite(ch_200K, LOW);
              // Cálculo da resistência a partir da leitura da entrada A3:
              float R1 = 200;
              voltage_A3 = analogRead(A3);
              buffer = voltage_A3 * V_in;
              float V_out = (buffer) / 1024.0;                          
              buffer = (V_in / V_out) - 1;
              float R2 = R1 * buffer;
            // Imprimir a informação de leitura de resistência
              if (R2 <= 200) {
                lcd.clear();// Limpar o display
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 200K");// Informação da escala
                lcd.setCursor(0,1);  lcd.print("Res= "); //Resistência lida
                lcd.setCursor(5,1);  lcd.print(R2); //Resistência lida
                lcd.setCursor(11,1); lcd.print("Kohms");// Unidade
                delay(500);
                              } else {
                lcd.clear();
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 200K"); // Informação da escala
                lcd.setCursor(0,1);  lcd.print("Sem leitura de R");// Resistência maior que a escala
                delay(500);    
                                     }
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");// Informa a função hold
            delay(10000);               }
            }

            //Escala de 2Mohm: -----     ---------       -----------      ---------        -------       ---------- 
            if (digitalRead(Escala_1M)) {
              // Níveis elétricos das saídas digitais do arduino, para executar a leitura de resitência na escala 2Mohm 
              pinMode(ch_2K,INPUT); pinMode(ch_20K,INPUT);  pinMode(ch_200K,INPUT);  pinMode(ch_1M,OUTPUT);
              digitalWrite(Apply_voltage, HIGH); digitalWrite(ch_1M, LOW);
              // Cálculo da resistência a partir da leitura da entrada A3:
              float R1 = 1;
              voltage_A3 = analogRead(A3);
              buffer = voltage_A3 * V_in;
              float V_out = (buffer) / 1024.0;                          
              buffer = (V_in / V_out) - 1;
              float R2 = R1 * buffer;
            // Imprimir a informação de leitura de resistência
              if (R2 <= 2) {
                lcd.clear();// Limpar o display
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 2M");// Informação da escala
                lcd.setCursor(0,1);  lcd.print("Res= "); //Resistência lida
                lcd.setCursor(5,1);  lcd.print(R2); //Resistência lida
                lcd.setCursor(11,1); lcd.print("Mohms");// Unidade
                delay(500);
                              } else {
                lcd.clear();
                lcd.setCursor(0, 0); lcd.print("ESCALA: 0 - 2M"); // Informação da escala
                lcd.setCursor(0,1);  lcd.print("Sem leitura de R");// Resistência maior que a escala
                delay(500);    
                                     }
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");// Informa a função hold
            delay(10000);               }
            }
            // Teste de continuidade:  -----     ---------       -----------      ---------        -------       ---------- 
            if (digitalRead(Escala_continuidade)) {
            pinMode(ch_2K,OUTPUT); pinMode(ch_20K,OUTPUT);  pinMode(ch_200K,OUTPUT);  pinMode(ch_1M,OUTPUT);
            digitalWrite(Apply_voltage, HIGH);digitalWrite(ch_2K, LOW);digitalWrite(ch_20K, LOW);digitalWrite(ch_200K, LOW);digitalWrite(ch_1M, LOW);
            voltage_A3 = analogRead(A3);
            lcd.clear(); // Serve para limpar a tela do display
            lcd.setCursor(0,0); lcd.print("Teste de cont.");
            if (voltage_A3>=1000 ){ lcd.setCursor(0,1); lcd.print("Circuito fechado"); digitalWrite(led_testeCC, HIGH);}
            else { lcd.setCursor(0,1); lcd.print("Circuito aberto"); digitalWrite(led_testeCC, LOW);}
            delay(500);
            }
            } while (digitalRead(menu));
             lcd.clear(); // Serve para limpar a tela do display
             lcd.setCursor(1,0); lcd.print("Voltar ao Menu");
             delay(2000);
             lcd.clear(); // Serve para limpar a tela do display
             digitalWrite(led_testeCC, LOW);
             break;
//- Wattimetro CC: ---------------------------------------------------------
       case 4: 
            do{
            //Medição de tensão:
            int ii=0;
            int sensorValue = analogRead(A0); // Leitura da porta analógica do arduino A0
            int escalaValue =  analogRead(A1); // Leitura da porta analógica do arduino A1
            if (escalaValue>400 && escalaValue<800 ) ii=1;
            if (escalaValue>0 && escalaValue<400 ) ii=2;
            //Serial.println(escalaValue);  Serial.println(ii);
            float voltage_A0 = sensorValue *Escala_tensao[ii]* (V_max_REF[ii] / 1023.0);    // Declaração da variável Tensão
            //Medição de corrente:
            float corrente_SUM = 0;
              for (int i = 0; i < NumAmostras; i++) {    // 100 amostras de leitura analógica do sensor ACS712 para maior precisão
                voltage_A2 = analogRead(A2) * 5.0 / 1023.0;
                corrente_SUM += (voltage_A2 - 2.4955) / Sensibilidadcc;}// Valor da sensibilidade é de 66mV/A
            float corrente = (corrente_SUM / NumAmostras);
            // Cálculo da potência
            float potencia = voltage_A0*corrente;
            // Imprimir no display LCD as informações de potência
            lcd.clear();
            lcd.setCursor(0, 0);lcd.print("Potencia CC");
            lcd.setCursor(0, 1);lcd.print("P = ");
            lcd.setCursor(7, 1);lcd.print(potencia);
            lcd.setCursor(15, 1);lcd.print("W");
            delay(500);
            if (digitalRead(enter)==LOW){
            lcd.setCursor(12, 0);lcd.print("Hold");
            delay(10000);
            }
            } while (digitalRead(menu));
             lcd.clear(); // Serve para limpar a tela do display
             lcd.setCursor(1,0); lcd.print("Voltar ao Menu");
             delay(2000);
             lcd.clear(); // Serve para limpar a tela do display
            break;
//--------------------------------------------------------------------------    
    }
}
