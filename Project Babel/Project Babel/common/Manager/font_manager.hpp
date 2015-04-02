//Checked 1




#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP




class FontManager : private TextRender
{


	char text[256];


	FloatingText *fText;


public:

	inline FontManager(GameObject * g_obj){ this->Init(g_obj); }
	inline ~FontManager(){ this->Clean(); }
    void Init(GameObject * g_obj);
	void Render(Controller * ctrl, GameObject * g_obj);
	void Clean();

};



#endif