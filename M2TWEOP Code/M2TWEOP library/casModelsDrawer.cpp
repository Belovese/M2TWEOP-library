#include "pch.h"
#include "casModelsDrawer.h"
#include "globals.h"
#include "realGameTypes.h"

#include "stratModelsChange.h"
namespace casModelsDrawer
{
	struct casModelRecS
	{
		casModelRecS(UINT32 id, int x, int y, float size)
			:ModelId(id), X(x), Y(y), Size(size)
		{

		}
		UINT32 ModelId = 0;
		int X = 0;
		int Y = 0;
		float Size = 1.0f;
	};

	struct casModelsDrawerS
	{
		std::shared_mutex casMutex;
		vector<unique_ptr<casModelRecS>>objects;
	}data;

	void addCasModelToDrawList(UINT32 modelId, int x, int y, float sizeMultiplier)
	{
		std::unique_lock<std::shared_mutex> lock(data.casMutex);
		data.objects.emplace_back(std::make_unique<casModelRecS>(modelId, x, y, sizeMultiplier));

		return;
	}

	void removeCasModelFromDrawList(UINT32 modelId)
	{
		std::unique_lock<std::shared_mutex> lock(data.casMutex);
		for (UINT32 i = 0; i < data.objects.size(); i++)
		{
			if (data.objects[i]->ModelId == modelId)
			{
				data.objects.erase(data.objects.begin() + i);
				i--;
			}
		}

		return;
	}

	void drawModel(const unique_ptr<casModelRecS>& model)
	{
		typedef void(__thiscall* SetModelForDraw)(void* modelDisplay, model_Rigid* model, float* matrix);
		typedef void(__cdecl* DoSomeWithCoordsGetZ)(float* xzy, float x, float y, float some1, float some2, float some3);

		SetModelForDraw setModelForDraw = nullptr;
		DoSomeWithCoordsGetZ doSomeWithCoordsGetZ = nullptr;
		void* modelDisp = nullptr;
		if (globals::dataS.gameVersion == 2)//steam
		{
			setModelForDraw = (SetModelForDraw)0x009e34e0;
			modelDisp = (void*)0x01982130;
			doSomeWithCoordsGetZ = (DoSomeWithCoordsGetZ)0x0098eb30;
		}
		else
		{
			setModelForDraw = (SetModelForDraw)0x009e2950;
			modelDisp = (void*)0x019cb260;
			doSomeWithCoordsGetZ = (DoSomeWithCoordsGetZ)0x0098dfb0;
		}

		float matrix[16]{ 0 };
		matrix[0] = model->Size;
		matrix[5] = model->Size;
		matrix[10] = model->Size;
		matrix[12] = model->X + 0.2f;
		matrix[14] = model->Y + 0.2f;
		matrix[15] = 1;

		auto* modelCasP = stratModelsChange::getModel(model->ModelId);
		if (modelCasP == nullptr)
		{
			return;
		}
		float coords[3]{ 0 };
		doSomeWithCoordsGetZ(coords, model->X + 0.2f, model->Y + 0.2f, 1, 1, 0);
		matrix[13] = coords[1];
		setModelForDraw(modelDisp, modelCasP, matrix);
	}


	void __stdcall drawModels()
	{
		std::shared_lock<std::shared_mutex> lock(data.casMutex);
		for (const unique_ptr<casModelRecS>& casModel : data.objects)
		{
			drawModel(casModel);
		}
	}


};
