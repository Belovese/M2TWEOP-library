///
//![Lua logo](../Lua.png)
//M2TWEOP structures and functions. There are not many examples and descriptions here. Also note that the examples do not include many of the checks that would be required when creating modifications.
//@module LuaPlugin
//@author youneuoy
//@license GPL-3.0
#include "pch.h"
#include "luaPlugin.h"
#include "mapTextDrawer.h"
#include <d3dx9mesh.h>
struct text3DDrawable
{
	LPD3DXMESH textMesh;
	DWORD color;
	float xCoord;
	float yCoord;
	float zCoord;
	float xSize = 1;
	float ySize = 1;
	float zSize = 1;
	float xRoll = 1;
	float yRoll = 90;
	float zRoll = 1;
	bool isDrawOnce;
	bool isDeleteNeeded;
};
void luaPlugin::initEop3dObjects()
{
	struct
	{
		sol::table M2TWEOP3dObjectsTable;
		sol::usertype<text3DDrawable> text3dDrawable;
	}tables;

	///M2TWEOP3dObjects
	//@section M2TWEOP3dObjects

	/***
	Basic M2TWEOP3dObjects table. Contains descriptions of m2tweop simple 3d objects feathures. Not call this functions onPluginLoad(graphics system not initialized here yet). 

	@tfield MakeTextFont MakeTextFont
	@tfield MakeTextFont DeleteTextFont
	@tfield Make3dText Make3dText

	@table M2TWEOP3dObjects
	*/

	tables.M2TWEOP3dObjectsTable = luaState.create_table("M2TWEOP3dObjects");

	/***
	Create new 3d text font. Fonts using for creating text(you not need font after it)
	@function M2TWEOP3dObjects.MakeTextFont
	@tparam string fontName name of font
	@tparam int weight weight of font(optional). Default - 400
	@tparam bool isItalic is font italic(optional). Default - true
	@treturn int fondID ID of created font.

	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	*/
	tables.M2TWEOP3dObjectsTable.set_function("MakeTextFont", &mapTextDrawer::MakeTextFontLua);
	/***
	Delete 3d text font. Do it where not need anymore
	@function M2TWEOP3dObjects.DeleteTextFont
	@tparam int fondID ID of created font.

	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
		newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	M2TWEOP3dObjects.DeleteTextFont(newFont);
	*/
	tables.M2TWEOP3dObjectsTable.set_function("DeleteTextFont", &mapTextDrawer::deleteTextFont);
	/***
	Create line of 3d text.
	@function M2TWEOP3dObjects.Make3dText
	@tparam int fondID ID of created font.
	@tparam string text utf8 string with text.
	@treturn Eop3dText text utf8 string with text.

	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	M2TWEOP3dObjects.DeleteTextFont(newFont);
	*/
	tables.M2TWEOP3dObjectsTable.set_function("Make3dText", &mapTextDrawer::makeText);

	///3dText
	//@section Eop3dText

	/***
	3d text object. Not use it onPluginLoad(graphics system not initialized here yet).  
	@tfield float xSize default value 1
	@tfield float ySize default value 0.2
	@tfield float zSize	default value 1
	@tfield float xRoll default value 1
	@tfield float yRoll default value 90
	@tfield float zRoll default value 1
	@tfield float xCoord
	@tfield float yCoord
	@tfield float zCoord
	@tfield Scale Scale
	@tfield ChangeColor ChangeColor
	@tfield SetCoords SetCoords
	@tfield StartDrawing StartDrawing
	@tfield StopDrawing StopDrawing
	@tfield DrawOnce DrawOnce
	@tfield Delete Delete

	@table Eop3dText
	*/

	tables.text3dDrawable = luaState.new_usertype<text3DDrawable>("Eop3dText");

	tables.text3dDrawable.set("xSize", &text3DDrawable::xSize);
	tables.text3dDrawable.set("ySize", &text3DDrawable::ySize);
	tables.text3dDrawable.set("zSize", &text3DDrawable::zSize);

	tables.text3dDrawable.set("xRoll", &text3DDrawable::xRoll);
	tables.text3dDrawable.set("yRoll", &text3DDrawable::yRoll);
	tables.text3dDrawable.set("zRoll", &text3DDrawable::zRoll);

	tables.text3dDrawable.set("xCoord", &text3DDrawable::xCoord);
	tables.text3dDrawable.set("yCoord", &text3DDrawable::yCoord);
	tables.text3dDrawable.set("zCoord", &text3DDrawable::zCoord);
	/***
	Scale 3d text.
	@function Eop3dText:Scale
	@tparam float sizeMultiplier
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:Scale(0.3);
	*/
	tables.text3dDrawable.set_function("Scale", &mapTextDrawer::ScaleText);

	/***
	Set 3d text color.
	@function Eop3dText:ChangeColor
	@tparam int a
	@tparam int r
	@tparam int g
	@tparam int b
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:ChangeColor(255,0,255,177);
	*/
	tables.text3dDrawable.set_function("ChangeColor", &mapTextDrawer::ChangeTextColor);
	/***
	Set 3d text coords.
	@function Eop3dText:SetCoords
	@tparam float x
	@tparam float y
	@tparam float z
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:SetCoords(10,20,0.2);
	*/
	tables.text3dDrawable.set_function("SetCoords", &mapTextDrawer::SetTextDrawingCoords);
	/***
	Start draw text. Text go on display until stop function called
	@function Eop3dText:StartDrawing
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:SetCoords(10,20,0.2);
	newText:StartDrawing();
	*/
	tables.text3dDrawable.set_function("StartDrawing", &mapTextDrawer::StartDrawingText);
	/***
	Stop draw text.
	@function Eop3dText:StopDrawing
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:SetCoords(10,20,0.2);
	newText:StartDrawing();
	--some time we draw
	newText:StopDrawing();
	*/
	tables.text3dDrawable.set_function("StopDrawing", &mapTextDrawer::StopDrawingText);
	/***
	Draw text once(at one frame only). Can be called in draw loop. Not need call stop after it.
	@function Eop3dText:DrawOnce
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:SetCoords(10,20,0.2);
	newText:DrawOnce();
	*/
	tables.text3dDrawable.set_function("DrawOnce", &mapTextDrawer::drawingTextOnce);
	/***
	Delete text. Not use after it!
	@function Eop3dText:Delete
	@usage
	newFont = M2TWEOP3dObjects.MakeTextFont("Times New Roman");
	newText = M2TWEOP3dObjects.Make3dText(newFont, "Some text");
	newText:SetCoords(10,20,0.2);
	newText:DrawOnce();
	newText:Delete();
	*/
	tables.text3dDrawable.set_function("Delete", &mapTextDrawer::Delete3dText);
}