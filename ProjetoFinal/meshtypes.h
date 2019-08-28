/**
 * @brief Diferentes partes da plataforma com materiais diferentes
 *
 *  EST - estruturas metálicas
 *  EQU - equipamentos
 *  TUB - tubulações
 *  CIV - estruturas de concreto
 *
 */

enum class MeshTypes : unsigned int
{
    PLATAFORMA            = 0x00000000,
    EQU1                  = 0x00000001,
    EQU2                  = 0x00000002,
    EQU3                  = 0x00000004,
    EQU4                  = 0x00000008,
    EST1                  = 0x00000010,
    EST2                  = 0x00000020,
    TUB                   = 0x00000040,
    TUBDET                = 0x00000080,
    CIV                   = 0x00000100

};
