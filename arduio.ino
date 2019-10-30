// for Arduino Nano

#define MAX_PARAMETER_NUMBER 10 // パラメータ数の最大値(コマンドそのものも含む)
#define MAX_PARAMETER_LENGTH 10 // パラメータの文字数の最大値

#define LED 13

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
}

bool isNumber(char c)
{
  return '0' <= c && c <= '9';
}

bool isNumberString(char* c)
{
  return isNumber(c[0]) || ( isNumber(c[1]) && c[0] == '-' );
}

bool isPwmPort(int port)
{
  return
    port == 3 ||
    port == 5 ||
    port == 6 ||
    port == 9 ||
    port == 10 ||
    port == 11;
}

bool isDigitalPort(int port)
{
  return 0 <= port && port <= 19 && port != LED;
}

bool isAnalogPort(int port)
{
  return 0 <= port && port <= 7;
}

int toInt(const char src[])
{
  int i = 0;
  int r = 0;
  int sign = 1;

  if(src[0] == '-')
  {
    sign = -1;
    i = 1;
  }
  
  for( ; isNumber(src[i]) ; i++ )
  {
    r = r * 10 + src[i] - '0';
  }
  return r * sign;
}

unsigned int toString(int num, char* buf, unsigned int len)
{
  String s = String(num);
  s.toCharArray(buf,len);
  return s.length();
}

void send_nack()
{
  Serial.print("NACK\n");
}

void send_ack(const char param[MAX_PARAMETER_LENGTH],unsigned int len)
{
  Serial.print("ACK");
  if(len != 0)
  {
    Serial.print(",");
  }
  Serial.write(param,len);
  Serial.print("\n");
}

void handle_set(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  int port = toInt(command[1]);
  String io = command[2];
  if( param_number != 3 ||
      isNumberString(command[1]) == false ||
      isDigitalPort(port) == false)
  {
    send_nack();
    return;
  }
  if(io.equals("OUT")){
    pinMode(port,OUTPUT);
  }
  else if(io.equals("IN")){
    pinMode(port,INPUT_PULLUP);
  }
  else{
    send_nack();
    return;
  }
  send_ack(NULL,0);
}

void handle_read(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  int port = toInt(command[1]);
  if( param_number != 2 ||
      isNumberString(command[1]) == false ||
      isDigitalPort(port) == false)
  {
    send_nack();
    return;
  }

  int res = digitalRead(port);
  send_ack(res==HIGH?"1":"0",1);
}


void handle_reada(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  if( param_number != 1)
  {
    send_nack();
    return;
  }

  char results[16];
  for(int i = 0 ; i != 8 ; i++ )
  {
    results[i*2] = digitalRead(i)==HIGH?'1':'0';
    results[i*2+1] = ',';
  }
  send_ack(results,15);
  return;
}

void handle_aread(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  int port = toInt(command[1]);
  char buf[10];
  if( param_number != 2 ||
      isNumberString(command[1]) == false ||
      isAnalogPort(port) == false)
  {
    send_nack();
    return;
  }
  unsigned int len = toString(analogRead(port), buf, 10);
  send_ack(buf,len);
}

void handle_write(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  int port = toInt(command[1]);
  int hilo = toInt(command[2]);
  if( param_number != 3 ||
      isNumberString(command[1]) == false ||
      isNumberString(command[2]) == false ||
      ( hilo != 1 && hilo != 0 ) ||
      isDigitalPort(port) == false)
  {
    send_nack();
    return;
  }
  digitalWrite(port,hilo==1?HIGH:LOW);
  send_ack(NULL,0);
  return;
}

void handle_pwm(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  int port = toInt(command[1]);
  int duty = toInt(command[2]);

  if( param_number != 3 ||
      isNumberString(command[1]) == false ||
      isNumberString(command[2]) == false ||
      duty < 0 || duty > 255 ||
      isPwmPort(port) == false)
  {
    send_nack();
    return;
  }
  analogWrite(port,duty);
  send_ack(NULL,0);
  return;
}

void handle_command(char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH], int param_number)
{
  String c = command[0];
  if(c.equals("SET"))
  {
    handle_set(command,param_number);
  }
  else if(c.equals("READ"))
  {
    handle_read(command,param_number);
  }
  else if(c.equals("READA"))
  {
    handle_reada(command,param_number);
  }
  else if(c.equals("AREAD"))
  {
    handle_aread(command,param_number);
  }
  else if(c.equals("WRITE"))
  {
    handle_write(command,param_number);
  }
  else if(c.equals("PWM"))
  {
    handle_pwm(command,param_number);
  }
  else
  {
    send_nack();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int param_index = 0;
  int char_index = 0;
  bool loop_flag = true;

  
  char command[MAX_PARAMETER_NUMBER][MAX_PARAMETER_LENGTH];
  for(int i = 0 ; i != MAX_PARAMETER_NUMBER ; i++ )
  {
    for(int j = 0 ; j != MAX_PARAMETER_LENGTH ; j++ )
    {
      command[i][j] = '\0';
    }
  }
  
  while(loop_flag)
  {
    if(Serial.available() > 0)
    {
      char c;
      c = Serial.read();
      switch(c)
      {
      case '\n':
        param_index++;
        loop_flag = false;
        break;
      case ',':
        command[param_index][char_index] = '\0';
        param_index++;
        char_index = 0;
        break;
      default:
        command[param_index][char_index] = c;
        char_index++;
      }
    }
  }
  digitalWrite(LED,LOW);
  handle_command(command,param_index);
  digitalWrite(LED,HIGH);
}
