//
// Created by lobis on 11/24/2021.
//

#ifndef RADIATION_TRANSPORT_EXCEPTIONS_H
#define RADIATION_TRANSPORT_EXCEPTIONS_H

#include <exception>

// Exceptions
namespace exceptions {

inline class NoGeometryFileException : public std::exception {
    virtual const char* what() const throw() { return "No geometry file defined"; }
} NoGeometryFile;

inline class GeometryFileNotFoundException : public std::exception {
    virtual const char* what() const throw() { return "Geometry file not found"; }
} GeometryFileNotFound;

}  // namespace exceptions

#endif  // RADIATION_TRANSPORT_EXCEPTIONS_H
