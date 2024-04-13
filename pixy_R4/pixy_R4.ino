#include <R4Timer.h>
#include <Pixy2.h>
Pixy2 pixy;


byte tx_data[7]; // 7 btye 크기를 할당
byte temp_button = 0;
byte checksum = 0;
volatile int t2_flag = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pixy.init();
  pixy.changeProg("line");


  tx_data[0] = 0xFF;
  tx_data[1] = 0xFF;

  Timer1.initialize(50);
  Timer1.attachInterrupt(T2ISR);
  Timer1.start();
}

void loop() {
  if (t2_flag) {
    t2_flag = 0;
    int i, j, num;
    int8_t x0, x1, y0, y1;
    char buf[128];

      // 모든 라인 특징 받아오기
    pixy.line.getAllFeatures();

  // 라인 개수 저장
    num = pixy.line.numVectors;

  // 모든 라인 출력
    for (i = 0; i < num; i++) {
      x0 = pixy.line.vectors[i].m_x0;
      x1 = pixy.line.vectors[i].m_x1;
      y0 = pixy.line.vectors[i].m_y0;
      y1 = pixy.line.vectors[i].m_y1;

      sprintf(buf, "Line num: %d ", i);
      Serial.print(buf);
      sprintf(buf, "m_x0: %d, m_y0: %d, m_x1: %d, m_y1: %d", x0, y0, x1, y1);
      Serial.println(buf);
  }

  // 기울기 계산
    float slope = (float)(y1 - y0) / (x1 - x0);

    memcpy(&tx_data[2], &slope, 4);

    checksum = 0;
    for (int i = 2; i < 6; i++) checksum ^= tx_data[i]; // 2,3,4,5
    checksum += 1;
    tx_data[6] = checksum;

    Serial1.write(tx_data, 7);
  }
}

void T2ISR() {
  t2_flag = 1;
}
