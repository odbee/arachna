#pragma once
#include "HelperFunctions.h"

string				mText;
gl::TextureRef		mTextTexture;
vec2				mSize;
Font				mFont;

void render()
{
	TextBox tbox = TextBox().alignment(TextBox::LEFT).font(mFont).size(ivec2(mSize.x, TextBox::GROW)).text(mText);
	tbox.setColor(Color(1.0f, 0.65f, 0.35f));
	tbox.setBackgroundColor(ColorA(0.5, 0, 0, 1));
	ivec2 sz = tbox.measure();
	mTextTexture = gl::Texture2d::create(tbox.render());
}
