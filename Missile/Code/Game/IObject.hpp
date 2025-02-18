#pragma once

class IObject {
public:
    virtual ~IObject() noexcept = default;
    virtual int GetScore() noexcept = 0;

protected:
private:

};
