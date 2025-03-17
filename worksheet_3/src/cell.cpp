#include "cell.h"

/*
* Add a molecule (its id) to the cell.
*/
void Cell::add_molecule(int id) {
    this->molecules.push_back(id);
    ++this->n_molecules;
}