/*
 *  group.h  --  declare server class Sgroup
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_GROUP_H
#define TWIN_GROUP_H

#include "obj/fwd.h"
#include <Tw/datatypes.h>

/* Tgroup -- group of Tgadget:s */

class Sgroup : public Sobj {
public:
  TobjFn Fn;
  Tgroup Prev, Next; /* list in the same Tmsgport */
  Tmsgport MsgPort;
  /* Tgroup */
  Tgadget FirstG, LastG; /* list in this Tgroup */
  Tgadget SelectG;

private:
  Tgroup Init(Tmsgport Parent);

public:
  static Tgroup Create(Tmsgport Parent);

  /* obj */
  virtual void Delete() OVERRIDE;
  void Insert(Tmsgport owner, Tgroup prev, Tgroup next);
  virtual void Remove() OVERRIDE;

  /* group */
  void InsertGadget(Tgadget g);
  void RemoveGadget(Tgadget g);
  Tgadget GetSelectedGadget() const;
  void SetSelectedGadget(Tgadget g);
};

#endif /* TWIN_GROUP_H */
