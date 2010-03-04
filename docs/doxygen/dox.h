/* dox.h - This file contains extra information for the
 * doxygen documentation generation system. */

/** @mainpage MADTraC Framework Source Documentation
 *
 * @section Introduction
 *
 * MADTraC is, in some sense, an evolution of 
 * <A href="http://glswarm.sourceforge.net">glSwarm</A>.
 *
 * @section quick_and_dirty Quick and Dirty
 *
 * The MT framework has four modules:
 *  - @ref MT_Core "MT Core"
 *  - @ref MT_GUI "MT GUI"
 *  - @ref MT_Tracking "MT Tracking"
 *  - @ref MT_Robot "MT Robot"
 *
 *
 *
 */

/** @defgroup MT_Core MT Core
 *
 * @brief Non-GUI data structures and help routines.
 *
 */

/** @defgroup MT_GUI MT GUI
 *
 * @brief Base classes for GUI interface
 *
 * */

/** @defgroup MT_Tracking MT Tracking
 *
 * @brief Base classes and support for tracking applications
 *
 * */

/** @defgroup MT_Robot MT Robot
 *
 * @brief Base classes and support for robot applications
 *
 * */

/** @defgroup third_party Third Party
 *
 * @brief Externally included libraries and references.
 *
 * */

/*************************************************************/
/* Definitions of data types included only for documentation
 * purposes - that is, these are not meant to compile, just as input
 * to Doxygen for reference purposes.  This way we don't have to mark
 * up any of the 3rd party files.  */

/** @addtogroup third_party
 * @{
 */

/** @struct IplImage OpenCV
 *
 * This is the main image structure used in MT - mainly because it is
 * the main image structure used in OpenCV.  The definition is
 * included here for reference purposes. */
typedef struct IplImage {
    int  nSize;                    /**< sizeof(IplImage) */
    int  ID;                       /**< version (=0)*/
    int  nChannels;                /**< Most of OpenCV functions support 1,2,3 or 4 channels */
    int  alphaChannel;             /**< ignored by OpenCV */
    int  depth;                    /**< pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                                        IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    char colorModel[4];            /**< ignored by OpenCV */
    char channelSeq[4];            /**< ignored by OpenCV */
    int  dataOrder;                /**< 0 - interleaved color channels, 1 - separate color channels.
                                        cvCreateImage can only create interleaved images */
    int  origin;                   /**< 0 - top-left origin,
                                        1 - bottom-left origin (Windows bitmaps style) */
    int  align;                    /**< Alignment of image rows (4 or 8).
                                       OpenCV ignores it and uses widthStep instead */
    int  width;                    /**< image width in pixels */
    int  height;                   /**< image height in pixels */
    struct _IplROI *roi;           /**< image ROI. if NULL, the whole image is selected */
    struct _IplImage *maskROI;     /**< must be NULL */
    void  *imageId;                /**< must be NULL */
    struct _IplTileInfo *tileInfo; /**< must be null */
    int  imageSize;                /**< image data size in bytes
                                        (==image->height*image->widthStep
                                        in case of interleaved data)*/
    char *imageData;               /**< pointer to aligned image data */
    int  widthStep;                /**< size of aligned image row in bytes */
    int  BorderMode[4];            /**< ignored by OpenCV */
    int  BorderConst[4];           /**< ignored by OpenCV */
    char *imageDataOrigin;         /**< pointer to very origin of image data
                                        (not necessarily aligned) -
                                        needed for correct deallocation */
}
IplImage;

/* @} */
