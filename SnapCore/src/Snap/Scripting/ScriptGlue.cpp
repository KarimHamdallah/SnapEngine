#include "SnapPCH.h"
#include "ScriptGlue.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <glm/glm.hpp>


#define SNAP_ADD_INTERNAL_CALL(FuncName) mono_add_internal_call("SnapEngine.InternalCalls::" #FuncName, FuncName)

namespace Scripting
{
	static void CppFunc()
	{
		std::cout << "CppFunc Call From C# and Print With std::cout\n";
	}

	static float Vector3_Dot(glm::vec3* v1, glm::vec3* v2)
	{
		float result = glm::dot(*v1, *v2);
		std::cout << "Dot Product Calculated From C++ And Debgged From C++ std::cout = " << glm::dot(*v1, *v2) << "\n";
		return result;
	}


	void ScriptGlue::RegisterGlue()
	{
		// Internal Calls

		SNAP_ADD_INTERNAL_CALL(CppFunc);
		SNAP_ADD_INTERNAL_CALL(Vector3_Dot);
		
		/*
		mono_add_internal_call("SnapEngine.Main::CppFunction", CppFunc);
		mono_add_internal_call("SnapEngine.Main::Vector3Dot", Vector3_Dot);
		*/
	}
}