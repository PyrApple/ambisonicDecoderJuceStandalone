/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   _3dti_logo_png;
    const int            _3dti_logo_pngSize = 13508;

    extern const char*   ambisonicLogo_png;
    const int            ambisonicLogo_pngSize = 88492;

    extern const char*   icl_logo_png;
    const int            icl_logo_pngSize = 16556;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 3;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
