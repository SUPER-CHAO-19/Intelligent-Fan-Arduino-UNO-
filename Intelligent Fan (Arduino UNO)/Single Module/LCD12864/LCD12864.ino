#include <U8g2lib.h> //这个库需要自己安装

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /*clock*/4, /*data*/3, /*cs*/2 , /*reset*/5);//创建显示器实例

void setup() {
	u8g2.begin();			//显示器初始化
}

void loop(void) {
	u8g2.firstPage();		//此为显示循环, 更加详细的内容可以参照下面给出的链接
	do {
		u8g2.setFont(u8g2_font_ncenB14_tr);	//设置字体
		u8g2.drawStr(0,20,"Tem");		//在指定位置显示字符 MadonG
	} while ( u8g2.nextPage() );
	delay(1000);
}
