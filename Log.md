# Editor Log
The development of the editor is part of an assignment hence I'm required to make a log :(

---

Day 1:
- A bit sick but started work towards the end of the day.
- Found a couple of other bugs which surprisingly didn't cause any issues on Nvidia but would've been really annoying to find out of nowhere.
- Started with the cmake requirements for adding multiple runtime projects so the editor can be its own runtime.
- DLL copying TODO.
- Ran into issues with the Intel Arc B580s OpenGL drivers behaving slightly different compared to the Nvidia ones.
- Found an issue that I thought was the Intel Arc treating the entire program as the only block when calling glGetActiveUniformBlockiv. But it ended up being nvidia that ignored an argument where I provided the wrong information. Never letting me catch the bug previously.

---

Day 2:
- Changed the target dir for the binaries to [Project Root]/bin to make dlls easier to manage.
- For the future I will attempt to use install for managin this, but for now this works well enough.
- Added ImGui
- Had to fix linking and the setup for ImGui but it ended up working.
- Needed to dig into how to set up ImGui for multi-viewport and docking, but got it eventually working.
- Got a viewport with basic functionality.

---

Day 3:
- Spent most of the day debugging and fixing the viewport.
- Created an interface named iSurface to allow a me to have "virtual" windows, making the viewport easier to control whilst running on the same render pipeline as the game would.
- I also made the Editor folder not build if the Editor runtime wasn't included in the cmake config. As it would be unecessary overhead when building the game only.
- The future plan is to have the Editor be it's own static plugin.

---

