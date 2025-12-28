// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepLib_SimplifyShape_HeaderFile
#define _BRepLib_SimplifyShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>

//! Provides an algorithm to simplify a shape by merging coplanar faces.
//!
//! This algorithm analyzes a shape and attempts to merge adjacent faces
//! that are coplanar within a specified tolerance. This is useful for
//! reducing the complexity of shapes while maintaining their geometric
//! integrity.
//!
//! The algorithm works by:
//! 1. Identifying adjacent faces in the shape
//! 2. Checking if they are coplanar within the given tolerance
//! 3. Merging coplanar faces into single faces when possible
//! 4. Rebuilding the shape with simplified topology
//!
//! Example usage:
//! @code
//! TopoDS_Shape aShape = ...;
//! BRepLib_SimplifyShape aSimplifier(aShape, 0.001);
//! if (aSimplifier.IsDone())
//! {
//!   TopoDS_Shape aSimplified = aSimplifier.Shape();
//!   Standard_Integer aReduction = aSimplifier.FacesReduced();
//! }
//! @endcode
class BRepLib_SimplifyShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Standard_EXPORT BRepLib_SimplifyShape();

  //! Constructor that performs simplification on the given shape.
  //! @param[in] theShape the shape to simplify
  //! @param[in] theTolerance tolerance for coplanarity check (default: 1e-7)
  //! @param[in] theMergeFaces whether to merge coplanar faces (default: true)
  Standard_EXPORT BRepLib_SimplifyShape(const TopoDS_Shape& theShape,
                                        const double        theTolerance  = 1e-7,
                                        const bool          theMergeFaces = true);

  //! Performs the simplification operation on the given shape.
  //! @param[in] theShape the shape to simplify
  //! @param[in] theTolerance tolerance for coplanarity check (default: 1e-7)
  //! @param[in] theMergeFaces whether to merge coplanar faces (default: true)
  Standard_EXPORT void Perform(const TopoDS_Shape& theShape,
                               const double        theTolerance  = 1e-7,
                               const bool          theMergeFaces = true);

  //! Returns true if the simplification operation was successful.
  //! @return true if operation completed successfully
  Standard_EXPORT bool IsDone() const;

  //! Returns the simplified shape.
  //! Should only be called if IsDone() returns true.
  //! @return the simplified shape
  Standard_EXPORT const TopoDS_Shape& Shape() const;

  //! Returns the number of faces that were merged/reduced.
  //! @return number of faces reduced
  Standard_EXPORT int FacesReduced() const;

  //! Returns the number of faces in the original shape.
  //! @return number of original faces
  Standard_EXPORT int OriginalFaceCount() const;

  //! Returns the number of faces in the simplified shape.
  //! @return number of simplified faces
  Standard_EXPORT int SimplifiedFaceCount() const;

protected:
  //! Analyzes the shape and counts faces.
  //! @param[in] theShape the shape to analyze
  void analyzeFaces(const TopoDS_Shape& theShape);

  //! Attempts to merge coplanar faces in the shape.
  //! @param[in] theShape the shape to process
  void mergeFaces(const TopoDS_Shape& theShape);

  //! Checks if two faces are coplanar within tolerance.
  //! @param[in] theFace1 first face
  //! @param[in] theFace2 second face
  //! @param[in] theTolerance tolerance for coplanarity check
  //! @return true if faces are coplanar
  bool areCoplanar(const TopoDS_Face& theFace1,
                   const TopoDS_Face& theFace2,
                   const double       theTolerance) const;

private:
  TopoDS_Shape myResultShape;      //!< The simplified result shape
  TopoDS_Shape myOriginalShape;    //!< The original input shape
  double       myTolerance;        //!< Tolerance for coplanarity check
  bool         myIsDone;           //!< Flag indicating if operation succeeded
  bool         myMergeFaces;       //!< Flag to enable/disable face merging
  int          myOriginalFaces;    //!< Count of faces in original shape
  int          mySimplifiedFaces;  //!< Count of faces in simplified shape
};

#endif // _BRepLib_SimplifyShape_HeaderFile
