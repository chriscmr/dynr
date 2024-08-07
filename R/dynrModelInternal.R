# TODO
# create dynrModel objects
# They should output a list like this
#model <- list(num_sbj=20,
#              dim_latent_var=2,
#              dim_obs_var=2,
#              dim_co_variate=2, 
#              num_regime=2,
#              isDiscretTime=0,
#              xstart=c(rep(log(.1), 4), log(10.0), log(10.0), -3.0, 9.0, -1.5, -0.5, 95.0,-.3,-.3),
#              num_func_param=13,
#              ub=c(rep(10, 6), rep(20, 4), 1000, 20, 20),
#              lb=c(rep(-10, 6), rep(-20, 4), 0, -20, -20)
#)

# Really all the model needs right now is
# num_regime, xstart, ub, lb
# Everything else can be gathered from the data.
# Importantly, the thing handed to the backend must
# remain a list exactly like the above.

.possible.times <- c('discrete', 'continuous')
checkAndProcessTimeArgument <- function(time){
	if(missing(time)){
		time <- "discrete"
	}
	if(!is.character(time)){
		stop("The 'time' argument must be a character (e.g. 'continuous' or 'discrete')")
	}
	time <- tolower(time)
	timeIndex <- pmatch(time, .possible.times)
	time <- .possible.times[timeIndex]
	return(time)
}


default.model.options <- list(xtol_rel=1e-7, stopval=-9999, ftol_rel=6.3e-12, 
                              ftol_abs=-1, maxeval=as.integer(10000), maxtime=-1)
#N.B. We may want to change these defaults.  Particularly, ftol_rel -> 6.3e-12

#' Do internal model preparation for dynr
#' 
#' Principally, this function takes a host of arguments and gives back
#' a list that importantly includes the function addresses.
#' 
#' @param num_regime An integer number of the regimes.
#' @param dim_latent_var An integer number of the latent variables.
#' @param xstart The starting values for parameter estimation.
#' @param ub The upper bounds of the estimated parameters.
#' @param lb The lower bounds of the estimated parameters.
#' @param options A list of NLopt estimation options. By default, xtol_rel=1e-7, stopval=-9999, ftol_rel=-1, ftol_abs=-1, maxeval=as.integer(-1), and maxtime=-1.
#' @param isContinuousTime A binary flag indicating whether the model is a continuous-time model (FALSE/0 = no; TRUE/1 = yes)
#' @param infile Input file name
#' @param outfile Output file name
#' @param compileLib Whether to compile the libary anew
#' @param verbose Logical flag for verbose output
#' @return A list of model statements to be passed to dynr.cook().
internalModelPrep <- function(num_regime, dim_latent_var, xstart, ub, lb, options=default.model.options, isContinuousTime, infile, outfile, compileLib, verbose){
	if(!is.list(options)){
		stop("'options' argument to internalModelPrep function must be a list.")
	}
	options <- processModelOptionsArgument(options)
	xlen <- length(xstart)
	ulen <- length(ub)
	llen <- length(lb)
	if( (xlen != ulen) || (xlen != llen) || (ulen != llen)){
		stop("Length of 'xstart', 'ub', and 'lb' must match.")
	}
	if( (length(num_regime) != 1) || (round(num_regime) != num_regime) ){
		stop("Number of regimes (num_regime) must be a single integer.")
	}
	if( (length(dim_latent_var) != 1) || (round(dim_latent_var) != dim_latent_var) ){
		stop("Number of latent variables (dim_latent_var) must be a single integer.")
	}
	
	#returns a list of addresses of the compiled model functions
	func_address=.C2funcaddress(isContinuousTime=isContinuousTime, infile=infile, outfile=outfile, verbose=verbose, compileLib=compileLib)
	return(list(num_regime=as.integer(num_regime), dim_latent_var=as.integer(dim_latent_var), xstart=xstart, ub=ub, lb=lb, isContinuousTime=isContinuousTime, num_func_param=as.integer(length(xstart)), func_address=func_address[[1]], options=options, libname=func_address[[2]]))
}

processModelOptionsArgument <- function(opt){
	if(!identical(opt, default.model.options)){
		nameMatch <- names(opt) %in% names(default.model.options)
		if( any(!nameMatch) ){
			msg <- paste("Tried to set invalid option(s): ", paste(names(opt)[!nameMatch], collapse=', '))
			stop(msg)
		}
		newopt <- default.model.options
		for(i in 1:length(opt)){
			newopt[[names(opt)[i]]] <- opt[[i]]
		}
		newopt$maxeval <- as.integer(newopt$maxeval)
		return(newopt)
	}else{
		return(opt)
	}
}

