#pragma once

#include <vector>

/*
* The cell class represent a single cell with its member molecules.
*/
class Cell{
    private:
        int n_molecules{};  // number of molecules in cell
        std::vector<int> molecules{};  // id of memeber molecules
    
    public:
        Cell() = default;
        
        int get_n_molecules() const { return this->n_molecules; }
        const std::vector<int>& get_molecules() const { return this->molecules; }
        void add_molecule(int id);
    };