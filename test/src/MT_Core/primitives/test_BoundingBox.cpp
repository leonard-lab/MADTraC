#include "MT_Test.h"

#include "MT/MT_Core/support/mathsupport.h"
#include "MT/MT_Core/primitives/BoundingBox.h"

void BB_Test(MT_BoundingBox B,
             double expect_xmin,
             double expect_xmax,
             double expect_ymin,
             double expect_ymax,
             double expect_zmin,
             double expect_zmax,
             bool expect_lock_xmin,
             bool expect_lock_xmax,
             bool expect_lock_ymin,
             bool expect_lock_ymax,
             bool expect_lock_zmin,
             bool expect_lock_zmax,
             int* p_in_status)
{
    if(!MT_IsEqual(B.xmin, expect_xmin)
       || !MT_IsEqual(B.xmax, expect_xmax)
       || !MT_IsEqual(B.ymin, expect_ymin)
       || !MT_IsEqual(B.ymax, expect_ymax)
       || !MT_IsEqual(B.zmin, expect_zmin)
       || !MT_IsEqual(B.zmax, expect_zmax))
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr,
                "    Bounding box data mismatch.  Have "
                "%f, %f, %f, %f, %f, %f.  Expected "
                "%f, %f, %f, %f, %f, %f\n",
                B.xmin, B.xmax, B.ymin, B.ymax, B.zmin, B.zmax,
                expect_xmin, expect_xmax, expect_ymin, expect_ymax,
                expect_zmin, expect_zmax);
    }

    double t;
    if(expect_lock_xmin)
    {
        t = B.xmin;
        B.ShowX(t - 1.0);
        if(!MT_IsEqual(B.xmin, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Failed expected xmin lock.\n");
        }
    }
    else
    {
        t = B.xmin - 1.0;
        B.ShowX(t);
        if(!MT_IsEqual(B.xmin, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Had xmin lock when not expected.\n");
        }
    }

    if(expect_lock_xmax)
    {
        t = B.xmax;
        B.ShowX(t + 1.0);
        if(!MT_IsEqual(B.xmax, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Failed expected xmax lock.\n");
        }
    }
    else
    {
        t = B.xmax + 1.0;
        B.ShowX(t);
        if(!MT_IsEqual(B.xmax, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Had xmax lock when not expected.\n");
        }
    }

    if(expect_lock_ymin)
    {
        t = B.ymin;
        B.ShowY(t - 1.0);
        if(!MT_IsEqual(B.ymin, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Failed expected ymin lock.\n");
        }
    }
    else
    {
        t = B.ymin - 1.0;
        B.ShowY(t);
        if(!MT_IsEqual(B.ymin, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Had ymin lock when not expected.\n");
        }
    }

    if(expect_lock_ymax)
    {
        t = B.ymax;
        B.ShowY(t + 1.0);
        if(!MT_IsEqual(B.ymax, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Failed expected ymax lock.\n");
        }
    }
    else
    {
        t = B.ymax + 1.0;
        B.ShowY(t);
        if(!MT_IsEqual(B.ymax, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Had ymax lock when not expected.\n");
        }
    }

    if(expect_lock_zmin)
    {
        t = B.zmin;
        B.ShowZ(t - 1.0);
        if(!MT_IsEqual(B.zmin, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Failed expected zmin lock.\n");
        }
    }
    else
    {
        t = B.zmin - 1.0;
        B.ShowZ(t);
        if(!MT_IsEqual(B.zmin, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Had zmin lock when not expected.\n");
        }
    }

    if(expect_lock_zmax)
    {
        t = B.zmax;
        B.ShowZ(t + 1.0);
        if(!MT_IsEqual(B.zmax, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Failed expected zmax lock.\n");
        }
    }
    else
    {
        t = B.zmax + 1.0;
        B.ShowZ(t);
        if(!MT_IsEqual(B.zmax, t))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr, "    Had zmax lock when not expected.\n");
        }
    }

}
             

int main(int argc, char** argv)
{
    int status = MT_TEST_SUCCESS;

    MT_TEST_START("BoundingBox: Ctors");

    /* default ctor should zero everything and enable showx, etc. */
    MT_BoundingBox b1;

    BB_Test(b1, 0, 0, 0, 0, 0, 0, false, false, false, false, false, false, &status);

    /* set ctor should not lock */
    MT_BoundingBox b2(-1, 1, -2, 2, -3, 3);
    BB_Test(b2, -1, 1, -2, 2, -3, 3, false, false, false, false, false, false, &status);

    /* copying should be simple */
    MT_BoundingBox b3 = b1;
    BB_Test(b3, 0, 0, 0, 0, 0, 0, false, false, false, false, false, false, &status);
    b3 = b2;
    BB_Test(b3, -1, 1, -2, 2, -3, 3, false, false, false, false, false, false, &status);

    MT_TEST_START("BoundingBox: Normal ShowX, ShowY, ShowZ");
    /* note that showing a value to an uninitialized BB sets min and
     * max values */
    b1.ShowX(-1);
    BB_Test(b1, -1, -1, 0, 0, 0, 0, false, false, false, false, false, false, &status);    
    b1.ShowX(1);
    BB_Test(b1, -1, 1, 0, 0, 0, 0, false, false, false, false, false, false, &status);
    b1.ShowX(-2);
    b1.ShowY(3);
    b1.ShowZ(10.5);
    b1.ShowZ(1000000.0);
    BB_Test(b1, -2, 1, 3, 3, 10.5, 1000000.0, false, false, false, false, false, false, &status);

    MT_TEST_START("BoundingBox: Locking");
    MT_BoundingBox b4;
    b4.SetXMinAndLock(-1);
    BB_Test(b4, -1, 0, 0, 0, 0, 0, true, false, false, false, false, false, &status);
    b4.SetXMaxAndLock(1);
    BB_Test(b4, -1, 1, 0, 0, 0, 0, true, true, false, false, false, false, &status);
    b4.SetYMinAndLock(-1);
    BB_Test(b4, -1, 1, -1, 0, 0, 0, true, true, true, false, false, false, &status);
    b4.SetYMaxAndLock(1);
    BB_Test(b4, -1, 1, -1, 1, 0, 0, true, true, true, true, false, false, &status);
    b4.SetZMinAndLock(-1);
    BB_Test(b4, -1, 1, -1, 1, -1, 0, true, true, true, true, true, false, &status);
    b4.SetZMaxAndLock(1);
    BB_Test(b4, -1, 1, -1, 1, -1, 1, true, true, true, true, true, true, &status);

    b2.SetXMinAndLock(0);
    BB_Test(b2, 0, 1, -2, 2, -3, 3, true, false, false, false, false, false, &status);    

    return status;
}
