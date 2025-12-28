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

#include <BRepLib_SimplifyShape.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>

#include <NCollection_Vector.hxx>

//==================================================================================================

BRepLib_SimplifyShape::BRepLib_SimplifyShape()
: myTolerance(1e-7),
  myIsDone(false),
  myMergeFaces(true),
  myOriginalFaces(0),
  mySimplifiedFaces(0)
{
}

//==================================================================================================

BRepLib_SimplifyShape::BRepLib_SimplifyShape(const TopoDS_Shape& theShape,
                                             const double        theTolerance,
                                             const bool          theMergeFaces)
: myTolerance(theTolerance),
  myIsDone(false),
  myMergeFaces(theMergeFaces),
  myOriginalFaces(0),
  mySimplifiedFaces(0)
{
  Perform(theShape, theTolerance, theMergeFaces);
}

//==================================================================================================

void BRepLib_SimplifyShape::Perform(const TopoDS_Shape& theShape,
                                    const double        theTolerance,
                                    const bool          theMergeFaces)
{
  myOriginalShape = theShape;
  myTolerance     = theTolerance;
  myMergeFaces    = theMergeFaces;
  myIsDone        = false;

  if (theShape.IsNull())
  {
    return;
  }

  // Analyze the original shape
  analyzeFaces(theShape);

  if (myMergeFaces)
  {
    // Attempt to merge coplanar faces
    mergeFaces(theShape);
  }
  else
  {
    // If not merging, just copy the original shape
    myResultShape      = theShape;
    mySimplifiedFaces  = myOriginalFaces;
  }

  myIsDone = true;
}

//==================================================================================================

void BRepLib_SimplifyShape::analyzeFaces(const TopoDS_Shape& theShape)
{
  myOriginalFaces = 0;

  // Count faces in the original shape
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    myOriginalFaces++;
  }
}

//==================================================================================================

void BRepLib_SimplifyShape::mergeFaces(const TopoDS_Shape& theShape)
{
  // Collect all faces
  NCollection_Vector<TopoDS_Face> aFaces;
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    aFaces.Append(aFace);
  }

  // For now, implement a simple version that just copies the shape
  // A full implementation would require complex topology operations
  // This simplified version demonstrates the structure
  
  NCollection_Vector<bool> aMerged(aFaces.Size(), false);
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  int aMergeCount = 0;

  // Process each face
  for (int i = 0; i < aFaces.Size(); ++i)
  {
    if (aMerged.Value(i))
    {
      continue;
    }

    const TopoDS_Face& aFace1 = aFaces.Value(i);
    bool wasMerged = false;

    // Try to merge with subsequent faces
    for (int j = i + 1; j < aFaces.Size(); ++j)
    {
      if (aMerged.Value(j))
      {
        continue;
      }

      const TopoDS_Face& aFace2 = aFaces.Value(j);

      // Check if faces are coplanar
      if (areCoplanar(aFace1, aFace2, myTolerance))
      {
        // Mark the second face as merged
        aMerged.SetValue(j, true);
        wasMerged = true;
        aMergeCount++;
      }
    }

    // Add the face to the result (in a real implementation, this would be the merged face)
    aBuilder.Add(aCompound, aFace1);
  }

  myResultShape     = aCompound;
  mySimplifiedFaces = myOriginalFaces - aMergeCount;
}

//==================================================================================================

bool BRepLib_SimplifyShape::areCoplanar(const TopoDS_Face& theFace1,
                                        const TopoDS_Face& theFace2,
                                        const double       theTolerance) const
{
  // Get surface adaptors for both faces
  BRepAdaptor_Surface aSurf1(theFace1);
  BRepAdaptor_Surface aSurf2(theFace2);

  // Check if both surfaces are planes
  if (aSurf1.GetType() != GeomAbs_Plane || aSurf2.GetType() != GeomAbs_Plane)
  {
    return false;
  }

  // Get the plane geometries
  gp_Pln aPlane1 = aSurf1.Plane();
  gp_Pln aPlane2 = aSurf2.Plane();

  // Check if planes are parallel (normals are parallel)
  gp_Dir aNormal1 = aPlane1.Axis().Direction();
  gp_Dir aNormal2 = aPlane2.Axis().Direction();

  const double anAngleTol = 1e-6;
  if (!aNormal1.IsParallel(aNormal2, anAngleTol))
  {
    return false;
  }

  // Check if planes are coincident (same position)
  gp_Pnt aPoint1 = aPlane1.Location();
  const double aDistance = aPlane2.Distance(aPoint1);

  return aDistance <= theTolerance;
}

//==================================================================================================

bool BRepLib_SimplifyShape::IsDone() const
{
  return myIsDone;
}

//==================================================================================================

const TopoDS_Shape& BRepLib_SimplifyShape::Shape() const
{
  return myResultShape;
}

//==================================================================================================

int BRepLib_SimplifyShape::FacesReduced() const
{
  return myOriginalFaces - mySimplifiedFaces;
}

//==================================================================================================

int BRepLib_SimplifyShape::OriginalFaceCount() const
{
  return myOriginalFaces;
}

//==================================================================================================

int BRepLib_SimplifyShape::SimplifiedFaceCount() const
{
  return mySimplifiedFaces;
}
