/*
 * ボタンを長押ししたときに録音モードに入る。
 * 
 * ループ再生は常にさせる。
 * 録音モード終了と同時にループを更新する。
 * 
 * テンポと音符が来るたびにそれぞれLEDを光らせている。
 * metro_test.inoでは、テンポのたびにHIGHとLOWを切り替えているが、
 * ここではmillis()で時間を計り、一定時間だけ光らせるようにしている。
 * 
 * クラスを使って、録音モードを動かすところまでやった。
 * 配列に正しく録音されるかは未確認。
 * 今後は、ピン番号もclassにいれて、
 * 録音モードをしめすLEDもclass内で制御するようにしたい。
 * 録音モードのLEDの光り方のバグを直す。
 * 
 * pitchについて
 * kick:21, snare:73, highhat:255
 * 
 * 何かしらの音がなっているとき、パワーLEDを光らせる。
 * 
 * 3つのトラックの音が高速で交互になることによって同時になるように見せかけている。
 * どれか一つだけを鳴らしたい場合は、Play()関数をnote.check() == 1のif文の中に入れればよい。
 * ただし、その場合、どれか一つのトラックのみの音が優先されて鳴る。
 * 
 * テンポをanalogReadで変えたい。
 * analogReadの範囲は0~1024。note当たりの長さを最大で128くらいにするのがちょうどいいので、
 * analogReadの値を8で割るとちょうどいいだろう。
 * だから、NUMBER_OF_NOTES_IN_A_BARは固定（１小節に32個の音符）。length_of_a_bar_timeを可変にする。
 * 具体的には、length_of_a_bar_time = analogRead(可変抵抗) / TEMPO_ADJUST * NUMBER_OF_NOTES_IN_A_BAR にする。
 */
 
#include <Metro.h>
#include "seq_controler.h"
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //PWM用
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))  //PWM用

#define NUMBER_OF_NOTES_IN_A_BAR 32      //1小節での音符の数
#define TEMPO_ADJUST 9 //length_of_a_bar_timeの取得時に、1024をいい感じにスケール変更するための値

//////// ピン番号たち ////////(pin11はfast PWMに使われている)
// input
#define P_LENGTH_OF_A_BAR_TIME 1  //length_of_a_bar_timeを決めるためのアナログピン番号
#define P_KICK_BUTTON          5  //kickボタン
#define P_SNARE_BUTTON         4  //snareボタン
#define P_HIGHHAT_BUTTON       3  //highhatボタン
#define P_ROTATE_TRACK_BUTTON  2  //トラックを入れ替えるボタンのピン番号
// output
#define P_LED_REC              6  //録音モードでオンになるピン番号
#define P_LED_TEMPO            8  //テンポごとに光らせる
#define P_LED_NOTE             7  //音符ごとに光らせる
#define P_POWER_LED_KICK       9  //kickのパワーLEDを光らせるピン番号
#define P_POWER_LED_SNARE     10  //snareのパワーLEDを光らせるピン番号
#define P_POWER_LED_HIGHHAT   12  //highhatのパワーLEDを光らせるピン番号


Metro tempo, note;   //テンポ、音符の間隔

unsigned int length_of_a_bar_time; //1小節の時間の長さ（可変）
//////// 録音用の変数たち ////////
int note_phase_int;           //1小節の中で何番目の音符かを示す
unsigned int t_breset, t_bstart, t_blength; //ボタンカウントのリセット時刻、ボタンを押された長さ測定用の変数
//////// LEDでのチェック用 ////////
unsigned int tt, ttb, tn, tnb; //LED点灯時刻、消灯時刻
//インスタンス生成。引数は、ボタンのピン番号、出力時の音程
SeqControler kick(P_KICK_BUTTON, 21), snare(P_SNARE_BUTTON, 73), highhat(P_HIGHHAT_BUTTON, 255);
// トラックの音符を入れ替える
void substitute();
unsigned int s_tb, s_t; //substitute判定のバウンシング対策。millis()を入れる


void setup() {
  // Timer2 PWM を高速にする
  cbi (TCCR2A, COM2A0);
  sbi (TCCR2A, COM2A1);
  sbi (TCCR2A, WGM20);
  sbi (TCCR2A, WGM21);
  cbi (TCCR2B, WGM22);
  sbi (TCCR2B, CS20);
  cbi (TCCR2B, CS21);
  cbi (TCCR2B, CS22);
  sbi(DDRB,3);
  
  // テンポを設定
  length_of_a_bar_time = analogRead(P_LENGTH_OF_A_BAR_TIME) / TEMPO_ADJUST * NUMBER_OF_NOTES_IN_A_BAR;
  tempo = Metro(length_of_a_bar_time);
  note = Metro(length_of_a_bar_time / NUMBER_OF_NOTES_IN_A_BAR); //length_of_a_bar_time / NUMBER_OF_NOTES_IN_A_BAR = (音符の長さ）
  
  pinMode(P_KICK_BUTTON, INPUT);
  pinMode(P_SNARE_BUTTON, INPUT);
  pinMode(P_HIGHHAT_BUTTON, INPUT);
  pinMode(P_ROTATE_TRACK_BUTTON, INPUT);

  //for test
  Serial.begin(9600);
  pinMode(P_LED_TEMPO, OUTPUT);
  pinMode(P_LED_NOTE, OUTPUT);
  pinMode(P_LED_REC, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(P_POWER_LED_KICK, OUTPUT);
  pinMode(P_POWER_LED_SNARE, OUTPUT);
  pinMode(P_POWER_LED_HIGHHAT, OUTPUT);

  note_phase_int = 0;
  s_tb = 0;
  s_t = 0;
}


void loop() {
  ///////////////////////////////////
  /////// tempoに合わせて光らせる //////
  ///////////////////////////////////
  if (tempo.check() == 1) {
    if (kick.m_pmode == 1) {
      kick.Record();                 //kick::録音モードのはじめをtempoに合わせる
      //digitalWrite(P_LED_REC, HIGH); //kick::録音モードであることを知らせるために光らせる
    }
    if (snare.m_pmode == 1) {
      snare.Record();                //snare::録音モードのはじめをtempoに合わせる
      //digitalWrite(P_LED_REC, HIGH); //snare::録音モードであることを知らせるために光らせる
    }
    if (highhat.m_pmode == 1) {
      highhat.Record();              //highhat::録音モードのはじめをtempoに合わせる
      //digitalWrite(P_LED_REC, HIGH); //highhat::録音モードであることを知らせるために光らせる
    }
    digitalWrite(P_LED_TEMPO, HIGH);
    Serial.println("tempo!");
    tt = millis();  //LED点灯時刻の記録
    
    length_of_a_bar_time = analogRead(P_LENGTH_OF_A_BAR_TIME) / TEMPO_ADJUST * NUMBER_OF_NOTES_IN_A_BAR; //length_of_a_bar_timeを変更
    tempo.interval(length_of_a_bar_time);      //実際のテンポの変更を適用
    note = Metro(length_of_a_bar_time / NUMBER_OF_NOTES_IN_A_BAR); //テンポの変更をnoteにも適用
  }
  ttb = millis();   //LED点灯からの時間記録
  if (ttb - tt >= 10) digitalWrite(P_LED_TEMPO, LOW); //LED消灯
  
  ////////////////////////////////////////////////
  /////// noteに合わせて光らせ、録音する //////
  ///////////////////////////////////////////////
  if (note.check() == 1) {
    digitalWrite(P_LED_NOTE, HIGH);
    tn = millis();  //LED点灯時刻の記録
    note_phase_int = (note_phase_int + 1) % NUMBER_OF_NOTES_IN_A_BAR; //いくつ目の音符かを数える
    
    //kick::録音モードのとき、録音する
    if (2 <= kick.m_pmode && kick.m_pmode <= NUMBER_OF_NOTES_IN_A_BAR) {
      kick.Record();
      //digitalWrite(P_LED_REC, HIGH);     //kick::録音モードであることをしらせるために光らせる
    }
    //snare::録音モードのとき、録音する
    if (2 <= snare.m_pmode && snare.m_pmode <= NUMBER_OF_NOTES_IN_A_BAR) {
      snare.Record();
      //digitalWrite(P_LED_REC, HIGH);     //snare::録音モードであることをしらせるために光らせる
    }
    //highhat::録音モードのとき、録音する
    if (2 <= highhat.m_pmode && highhat.m_pmode <= NUMBER_OF_NOTES_IN_A_BAR) {
      highhat.Record();
      //digitalWrite(P_LED_REC, HIGH);     //highhat::録音モードであることをしらせるために光らせる
    }
    //どれも録音モードでないとき、LEDを消す
    //if (kick.m_pmode == 0 && snare.m_pmode == 0 && highhat.m_pmode == 0) {
    //  digitalWrite(P_LED_REC, LOW);
    //}
    
    if (kick.m_notes[note_phase_int] == 0 && snare.m_notes[note_phase_int] == 0 && highhat.m_notes[note_phase_int] == 0) {
      OCR2A = 0; // どのトラックもなっていなければ出力を0にする。
    }
    //kickが鳴るとき、パワーLEDを光らせる
    if (kick.m_notes[note_phase_int] == 1) digitalWrite(P_POWER_LED_KICK, HIGH);
    else digitalWrite(P_POWER_LED_KICK, LOW);
    //snareが鳴るとき、パワーLEDを光らせる
    if (snare.m_notes[note_phase_int] == 1) digitalWrite(P_POWER_LED_SNARE, HIGH);
    else digitalWrite(P_POWER_LED_SNARE, LOW);
    //highhatが鳴るとき、パワーLEDを光らせる
    if (highhat.m_notes[note_phase_int] == 1) digitalWrite(P_POWER_LED_HIGHHAT, HIGH);
    else digitalWrite(P_POWER_LED_HIGHHAT, LOW);

    Serial.println(analogRead(1));// analogReadのテスト用
  }

  // 再生する
  snare.Play(note_phase_int);   //snare::録音したものを再生する
  highhat.Play(note_phase_int); //highhat::録音したものを再生する
  kick.Play(note_phase_int);    //kick::録音したものを再生する
  
  tnb = millis();   //LED点灯からの時間記録
  if (tnb - tn >= 10) digitalWrite(P_LED_NOTE, LOW); //LED消灯

  //////////////////////////////////////////////////////////////////////
  //////// ボタンの状態測定 //////////////////////////////////////////////
  /// グローバル変数t_startを全ての場合に使っているので、そこでのバグが心配。 ///
  /// メンバ変数m_t_startを使うことにすれば心配が解決できるかも。           ///
  //////////////////////////////////////////////////////////////////////
  //kick::ボタンが押されているかどうかによって、b_kickに1または0を代入
  if (digitalRead(P_KICK_BUTTON) == HIGH) {
    if (kick.m_bstate == 0) t_bstart = millis();
    kick.m_bstate = 1;
  } else kick.m_bstate = 0;
  //snare::ボタンが押されているかどうかによって、b_snareに1または0を代入
  if (digitalRead(P_SNARE_BUTTON) == HIGH) {
    if (snare.m_bstate == 0) t_bstart = millis();
    snare.m_bstate = 1;
  } else snare.m_bstate = 0;
  //highhat::ボタンが押されているかどうかによって、b_highhatに1または0を代入
  if (digitalRead(P_HIGHHAT_BUTTON) == HIGH) {
    if (highhat.m_bstate == 0) t_bstart = millis();
    highhat.m_bstate = 1;
  } else highhat.m_bstate = 0;

  ///////////////////////////////////
  //////// modeの切り替え ////////////
  ///////////////////////////////////
  t_blength = millis();
  //kick::ボタンを押し始めてから経過した時間がlength_of_a_bar_time以上で、かつ今もボタンが押されているとき、
  if (kick.m_bstate == 1 && t_blength - t_bstart > length_of_a_bar_time) {
    kick.m_pmode = 1;      //録音モードに切り替える
    Serial.write("record mode kick on\n");
  }
  //snare::ボタンを押し始めてから経過した時間がlength_of_a_bar_time以上で、かつ今もボタンが押されているとき、
  if (snare.m_bstate == 1 && t_blength - t_bstart > length_of_a_bar_time) {
    snare.m_pmode = 1;      //録音モードに切り替える
    Serial.write("record mode snare on\n");
  }
  //highhat::ボタンを押し始めてから経過した時間がlength_of_a_bar_time以上で、かつ今もボタンが押されているとき、
  if (highhat.m_bstate == 1 && t_blength - t_bstart > length_of_a_bar_time) {
    highhat.m_pmode = 1;      //録音モードに切り替える
    Serial.write("record mode highhat on\n");
  }

  s_t = millis();
  if (digitalRead(P_ROTATE_TRACK_BUTTON) == HIGH && s_t - s_tb > 100) {
    substitute();
    s_tb = millis();
  }
} 


void substitute() {
  bool temp;
  int j;
  for (j = 0; j < NUMBER_OF_NOTES_IN_A_BAR; j++) { 
    temp = highhat.m_notes[j];
    highhat.m_notes[j] = snare.m_notes[j];
    snare.m_notes[j] = kick.m_notes[j];
    kick.m_notes[j] = temp;
    
  }
}

