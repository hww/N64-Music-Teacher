/* Nintendo 64 Karaoke system configuration */
typedef struct {
		char name[16];	//�������� �������樨
		int size;	//������⢮ �⢥�⥩ � ��ப�
		int base1;	//������� ��� �� ������ 1
		int base2;	//������� ��� �� ������ 2
		int ticks;	//������⢮ ⨪�� �� �⢥���
		int chanels;	//������⢮ �४��
				}	files;

#define MAX_PITCH_U 7	//���ᨬ��쭮� ᬥ饭�� ⮭��쭮�� �����
#define MAX_PITCH_D -7	//���ᨬ��쭮� ᬥ饭�� ⮭��쭮�� ����
#define MAX_TEMPO 250	//���ᨬ���� ⥬�
#define MIN_TEMPO 40	//��������� ⥬�
#define STP_TEMPO 5	//��� ��������� ⥬��
#define MAX_FILE_SBK 9  //������⢮ MIDI 䠩��� � ࠡ�祬 SBK
#define LINES 4		//������⢮ ��ப � �뢮�� ��࠮��
#define KARA_CH0 14 	//����� 0 ��� ��࠮��
#define KARA_CH1 15 	//����� 1 ��� ��࠮��

files songs[MAX_FILE_SBK]=
	{	"A1",  4,48,48,192,1,
		"-2",  4,48,48,192,0,
		"A3",  4,48,24,192,1,		
		"A4",  4,48,24,192,1,
		"A5",  4,48,24,192,1,
		"A6",  4,48,24,192,1,
		"A7-2",4,48,48,120,2,
		"A8",  4,48,48,192,1,
		"-9",  4,48,24,192,0, };
		           