/*
 * sequencer_with_class2.inoのはじめに#defineしているNUMBER_OF_NOTES_IN_A_BARをこのファイルでは参照できないので、
 * あらたに#define NUMBER_OF_NOTES_IN_A_BAR 4と書いている。
 * ここで#defineする必要のない方法として、
 * .inoファイル内でのNUMBER_OF_NOTES_IN_A_BARを引数で渡して、class内でmallocして配列を作ればできそうではあるが、
 * 難しそうだったり、メモリのことも気にしないといけない気がするのでやめた。
 * 
 * とりあえずこれで思った通りの音が出るようになった。
 * 録音モードを示すLEDの光り方がおかしいという問題は残っている（大きな問題ではないけれど）。
 */

#ifndef _SEQ_CONTROLER_H_
#define _SEQ_CONTROLER_H_
#define NUMBER_OF_NOTES_IN_A_BAR 32

//////////////////////////
////// クラスの定義 ////////
//////////////////////////
class SeqControler {
  public:
    SeqControler(int pin, int pitch); //コンストラクタ。pin:ボタンのピン番号、pitch:出力時の音程
    void Record();         //録音する関数
    void Play(int note_phase_int);           //録音されているものを出力する関数
    int m_pmode; //モード判断の変数(1, 2, 3...:録音モード, 0:通常モード)
    bool m_bstate; //ボタンが押されたかの判定用
    int m_pitch;         //出力時のPWMの速さ（音程）
    bool m_notes[NUMBER_OF_NOTES_IN_A_BAR];  //それぞれのトラックのon, offを代入
  private:
    int m_bpin, m_outpin; //ボタンのピン番号、アウトプットのピン番号
    bool m_temp[NUMBER_OF_NOTES_IN_A_BAR];   //録音時に一時的に記録する配列
    
};

#endif

