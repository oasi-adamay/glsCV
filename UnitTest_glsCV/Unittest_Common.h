#pragma once

namespace UnitTest_glsCV
{

	bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff = 1e-3);

	bool AreEqual(Mat& mat0, Mat& mat1);


}