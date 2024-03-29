/// custom_termination_criterion.cpp
///
/// Created on: 11 Nov 2020
/// Author:     Fabian Meyer
/// License:    MIT

#include <lsqcpp/lsqcpp.hpp>

// Implement an objective functor.
struct ParabolicError
{
    static constexpr bool ComputesJacobian = false;

    template<typename Scalar, int Inputs, int Outputs>
    void operator()(const Eigen::Matrix<Scalar, Inputs, 1> &xval,
                    Eigen::Matrix<Scalar, Outputs, 1> &fval) const
    {
        // omit calculation of jacobian, so finite differences will be used
        // to estimate jacobian numerically
        fval.resize(xval.size() / 2);
        for(lsqcpp::Index i = 0; i < fval.size(); ++i)
            fval(i) = xval(i*2) * xval(i*2) + xval(i*2+1) * xval(i*2+1);
    }
};

// implement a termination condition
struct MyTerminationCondition
{
    // if this operator returns false, the optimization process is stopped.
    bool operator()(const lsqcpp::Index iteration,
                    const Eigen::VectorXd &xval,
                    const Eigen::VectorXd &fval,
                    const Eigen::MatrixXd &jacobian,
                    const Eigen::VectorXd &gradient,
                    const Eigen::VectorXd &step) const
    {
        // this does not make sense, but its an example!
        if(gradient.norm() > 1e-2 && step.norm() < 1e-2)
            return false;

        // if we have done many iterations and error is low then stop
        if(iteration > 100 && fval.norm() < 1e-2)
            return false;

        // we are getting out of bounds, stop here
        if(xval(0) > 100)
            return false;

        // just do something with the jacobian
        if(jacobian(0, 0) + jacobian(1, 1) < 1e-6)
            return false;

        if(iteration == 10)
            return false;

        return true;
    }
};

int main()
{
    // Create GradienDescent optimizer with Barzilai Borwein method
    lsqcpp::GaussNewtonX<double, ParabolicError> optimizer;

    // set the termination criterion
    optimizer.setCallback(MyTerminationCondition());

    // Set number of iterations as stop criterion.
    optimizer.setMaximumIterations(100);

    // Set the minimum length of the gradient.
    optimizer.setMinimumGradientLength(1e-6);

    // Set the minimum length of the step.
    optimizer.setMinimumStepLength(1e-6);

    // Set the minimum least squares error.
    optimizer.setMinimumError(0);

    // Turn verbosity on, so the optimizer prints status updates after each
    // iteration.
    optimizer.setVerbosity(4);

    // Set initial guess.
    Eigen::VectorXd initialGuess(4);
    initialGuess << 1, 2, 3, 4;

    // Start the optimization.
    auto result = optimizer.minimize(initialGuess);

    std::cout << "Done! Converged: " << (result.converged ? "true" : "false")
        << " Iterations: " << result.iterations << std::endl;

    // do something with final function value
    std::cout << "Final fval: " << result.fval.transpose() << std::endl;

    // do something with final x-value
    std::cout << "Final xval: " << result.xval.transpose() << std::endl;

    return 0;
}
