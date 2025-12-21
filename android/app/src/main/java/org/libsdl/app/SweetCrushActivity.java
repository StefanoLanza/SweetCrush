package org.libsdl.app;

import org.libsdl.app.SDLActivity;

public class SweetCrushActivity extends SDLActivity {
    protected String[] getLibraries() {
        return new String[] { "SDL3", "SDL3-mixer", "SDL3-image" };
    }
}