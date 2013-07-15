classdef qucstrans < qucs
    % interface to the qucs transient circuit solver
    %
    
    % Copyright Richard Crozier 2013
    
    properties ( SetAccess = protected, GetAccess = public )
        
        isinitialised; % for marking the solver as initialised
        
    end
    
    methods
        
        %% Constructor - Create a new instance of the class
        function this = qucstrans(netlist)
            
            this = this@qucs(@m_trsolver_interface_mex, netlist);
            
            this.isinitialised = false;
            
        end

        %%%%% class methods
        
        function debug(this)
            this.cppcall('debug');
        end
        
        function printx(this)
            this.cppcall('printx');
        end
        
    end
    
end