/*
 * ここでも新たに#define NUMBER_OF_NOTES_IN_A_BAR 4している。詳しいことはseq_controler.h内にコメントしている。
 * Play(int note_phase_int)内での出力音程は現状では適当に決めている。変数でおければもっといいのかもしれない。
 */
#include "seq_controler.h"
#include "Arduino.h"
#define NUMBER_OF_NOTES_IN_A_BAR 32
#define P_OUT 11 //kick, snare, highhatのアウトプット用のピン
#define P_LED_REC   6           //録音モードでオンになるピン番号

//////////////////////////////
// コンストラクタ（初期化処理） //
//////////////////////////////
SeqControler::SeqControler(int pin, int pitch) {
  m_bpin = pin;               //メンバ変数にボタンのピン番号を代入
  m_bstate = 0;
  m_pitch = pitch;            //メンバ変数に出力時の音程を入力
  pinMode (m_bpin, INPUT);
}

/////////////////////////////////////////////////////////////////////////////
// fnoteの値にあった配列要素にボタン(b_kick, b_snare, b_highhat)の状態を記録する。//
// fnote == 3のとき、録音終了とともにmode = 0とし、配列を更新。////////////////////
/////////////////////////////////////////////////////////////////////////////
void SeqControler::Record() {
  Serial.println("Record func is on\n"); //テストのために表示
  digitalWrite(P_LED_REC, HIGH); //録音モードであることを知らせるために光らせる
  m_temp[m_pmode - 1] = digitalRead(m_bpin); // ボタンの状態を一時的に記録
  m_pmode++;
  if (m_pmode == NUMBER_OF_NOTES_IN_A_BAR + 1) {
    int i;
    for (i = 0; i < NUMBER_OF_NOTES_IN_A_BAR; i++) {
      m_notes[i] = m_temp[i]; // 一時的に記録した状態をm_notesに代入
      m_temp[i] = 0;          // 次の測定のために、全てLOWに変えておく
    }
    // modeを0に戻し、録音モード終了
    m_pmode = 0;
    Serial.println("Record mode kick finished");
    digitalWrite(P_LED_REC, LOW); //録音モードのLEDを消す
  }
  
}

////////////////////////////////////////////////////
// 録音した配列を出力する。音符は第１小節のみ1から始まり、//
// 他の小節は0, 1, 2, 3,..., NUMBER_OF_NOTES_IN_A_BAR - 1とすすむ。      //
////////////////////////////////////////////////////
void SeqControler::Play(int note_phase_int) {
  if (m_notes[note_phase_int] == 1) {
    OCR2A = m_pitch; // OCR2A という変数に 0 ~ 255までの値を入れると、pin11からPWMが出力される
  }
}

