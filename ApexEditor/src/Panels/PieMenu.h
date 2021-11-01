#pragma once

namespace ImGui {

	/**
	 * Author: thennequin (GitHub)
	 * Source: https://gist.github.com/thennequin/64b4b996ec990c6ddc13a48c6a0ba68c
	 */
	bool BeginPiePopup( const char* pName, int iMouseButton = 0 );
	void EndPiePopup();

	bool PieMenuItem( const char* pName, bool bEnabled = true );
	bool BeginPieMenu( const char* pName, bool bEnabled = true );
	void EndPieMenu();

}
